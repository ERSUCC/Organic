#include "../include/parse.h"

namespace Parser {

#define CALL(func) [](const SourceLocation& location, ArgumentList* arguments) { return UniqueToken<Call>(new func(location, arguments)); }

static std::unordered_map<std::string, std::function<UniqueToken<Call> (const SourceLocation&, ArgumentList*)>> libraryFunctions =
{
    { "time", CALL(Time) },
    { "hold", CALL(Hold) },
    { "lfo", CALL(LFO) },
    { "sweep", CALL(Sweep) },
    { "sequence", CALL(Sequence) },
    { "repeat", CALL(Repeat) },
    { "random", CALL(Random) },
    { "limit", CALL(Limit) },
    { "trigger", CALL(Trigger) },
    { "if", CALL(If) },
    { "all", CALL(All) },
    { "any", CALL(Any) },
    { "none", CALL(None) },
    { "min", CALL(Min) },
    { "max", CALL(Max) },
    { "round", CALL(Round) },
    { "absolute", CALL(Absolute) },
    { "sine", CALL(Sine) },
    { "square", CALL(Square) },
    { "triangle", CALL(Triangle) },
    { "saw", CALL(Saw) },
    { "noise", CALL(Noise) },
    { "sample", CALL(Sample) },
    { "oscillator", CALL(Oscillator) },
    { "granulate", CALL(Granulate) },
    { "group", CALL(Group) },
    { "effect-group", CALL(EffectGroup) },
    { "delay", CALL(Delay) },
    { "comb", CALL(Comb) },
    { "all-pass", CALL(AllPass) },
    { "low-pass", CALL(LowPass) },
    { "reverb", CALL(Reverb) }
};

ParserContext::ParserContext(ParserContext* parent, const ContextType& type, const std::string name, const std::vector<UniqueToken<InputDef>>& inputs) :
    parent(parent), type(type), name(name)
{
    for (const UniqueToken<InputDef>& input : inputs)
    {
        this->inputs[input->string()] = input.get();
    }
}

ParserContext::~ParserContext()
{
    for (const Token* instruction : instructions)
    {
        delete instruction;
    }
}

const VariableDef* ParserContext::addVariable(const Identifier* token, const Token* value)
{
    const VariableDef* variable = new VariableDef(token->location, value);

    variables[token->string()] = variable;

    instructions.push_back(variable);

    return variable;
}

const FunctionDef* ParserContext::addFunction(const Identifier* token, std::vector<UniqueToken<InputDef>>& inputs, const Program* program)
{
    std::vector<const InputDef*> owned;

    for (UniqueToken<InputDef>& input : inputs)
    {
        owned.push_back(input.release());
    }

    const FunctionDef* function = new FunctionDef(token->location, owned, program);

    functions[token->string()] = function;

    instructions.push_back(function);

    return function;
}

const Identifier* ParserContext::findIdentifier(const Identifier* token)
{
    if (inputs.count(token->string()))
    {
        const InputDef* input = inputs[token->string()];

        used.insert(input);

        return new InputRef(token->location, input);
    }

    if (variables.count(token->string()))
    {
        const VariableDef* variable = variables[token->string()];

        used.insert(variable);

        return new VariableRef(token->location, variable);
    }

    if (functions.count(token->string()))
    {
        const FunctionDef* function = functions[token->string()];

        used.insert(function);

        return new FunctionRef(token->location, function);
    }

    if (parent)
    {
        if (const Identifier* identifier = parent->findIdentifier(token))
        {
            return identifier;
        }
    }

    throw OrganicParseException("No variable, input, or function exists with the name \"" + token->string() + "\".", token->location);
}

const FunctionDef* ParserContext::findFunction(const Identifier* token)
{
    if (functions.count(token->string()))
    {
        const FunctionDef* function = functions[token->string()];

        used.insert(function);

        return function;
    }

    if (parent)
    {
        if (const FunctionDef* function = parent->findFunction(token))
        {
            return function;
        }
    }

    throw OrganicParseException("No function exists with the name \"" + token->string() + "\".", token->location);
}

void ParserContext::addInstruction(const Token* instruction)
{
    instructions.push_back(instruction);
}

void ParserContext::checkNameConflicts(const Identifier* token) const
{
    const std::string name = token->string();

    if (inputs.count(name))
    {
        throw OrganicParseException("An input already exists with the name \"" + name + "\".", token->location);
    }

    if (variables.count(name))
    {
        throw OrganicParseException("A variable already exists with the name \"" + name + "\".", token->location);
    }

    if (functions.count(name))
    {
        throw OrganicParseException("A function already exists with the name \"" + name + "\".", token->location);
    }

    if (parent)
    {
        parent->checkNameConflicts(token);
    }
}

bool ParserContext::checkRecursive(const ContextType& type, const Identifier* token) const
{
    return (this->type == type && name == token->string()) || (parent && parent->checkRecursive(type, token));
}

void ParserContext::checkUsage() const
{
    for (const std::pair<std::string, const InputDef*>& pair : inputs)
    {
        if (!used.count(pair.second))
        {
            Utils::parseWarning("Unused input \"" + pair.first + "\".", pair.second->location);
        }
    }

    for (const std::pair<std::string, const VariableDef*>& pair : variables)
    {
        if (!used.count(pair.second))
        {
            Utils::parseWarning("Unused variable \"" + pair.first + "\".", pair.second->location);
        }
    }

    for (const std::pair<std::string, const FunctionDef*>& pair : functions)
    {
        if (!used.count(pair.second))
        {
            Utils::parseWarning("Unused function \"" + pair.first + "\".", pair.second->location);
        }
    }
}

const Program* ParserContext::buildProgram(const SourceProvider* source)
{
    const Program* program = new Program(SourceLocation(source, 0, source->length()), instructions);

    instructions.clear();

    return program;
}

const Program* Parser::parseSource(const SourceProvider* source)
{
    ParserContext* context = new ParserContext(nullptr, ContextType::Program, "", {});

    std::unordered_set<Path, Path::Hash, Path::Equals> includedPaths = { source->path() };

    Parser* parser = new Parser(source, context, includedPaths);

    try
    {
        parser->parseProgram();

        context->checkUsage();
    }

    catch (const OrganicException& e)
    {
        delete parser;
        delete context;

        throw;
    }

    delete parser;

    const Program* program = context->buildProgram(source);

    delete context;

    return program;
}

Parser::Parser(const SourceProvider* source, ParserContext* context, std::unordered_set<Path, Path::Hash, Path::Equals>& includedPaths) :
    source(source), context(context), includedPaths(includedPaths) {}

Parser::~Parser()
{
    while (context->parent)
    {
        const ParserContext* inner = context;

        context = context->parent;

        delete inner;
    }

    delete tokens;
}

const void Parser::parseProgram()
{
    tokens = Tokenizer::tokenize(source);

    while (tokens->peek()->string() == "include" && tokens->peek<OpenParenthesis>(1))
    {
        parseInclude();
    }

    while (!tokens->peek()->eof())
    {
        parseInstruction();
    }
}

const void Parser::parseInclude()
{
    const SourceLocation location = tokens->peek()->location;

    const UniqueToken<String> str = tokens->drop(2)->require<String>("file path");

    tokens->expect<CloseParenthesis>("\")\"");

    const std::filesystem::path file = Path::formatPath(str->str);

    if (file.empty())
    {
        Utils::parseWarning("This include does not specify a source file, it will have no effect.", location);

        return;
    }

    const Path sourcePath = source->path();
    const Path includePath = Path::beside(file, sourcePath);

    if (!includePath.exists())
    {
        throw OrganicParseException("Source file \"" + str->str + "\" does not exist.", location);
    }

    if (!includePath.isFile())
    {
        throw OrganicParseException("\"" + str->str + "\" is not a file.", location);
    }

    if (sourcePath.string() == includePath.string())
    {
        Utils::parseWarning("Source file \"" + str->str + "\" is the current file, this include will be ignored.", location);

        return;
    }

    if (includedPaths.count(includePath))
    {
        Utils::parseWarning("Source file \"" + str->str + "\" has already been included, this include will be ignored.", location);

        return;
    }

    includedPaths.insert(includePath);

    const FileProvider* includeSource = FileProvider::create(includePath);

    Parser* parser = new Parser(includeSource, context, includedPaths);

    try
    {
        parser->parseProgram();
    }

    catch (const OrganicException& e)
    {
        delete includeSource;

        throw;
    }

    delete parser;
    delete includeSource;
}

const void Parser::parseInstruction()
{
    if (tokens->peek<Equals>(1))
    {
        parseAssign();

        return;
    }

    if (tokens->peek<OpenParenthesis>(1))
    {
        size_t current = 0;
        size_t depth = 0;

        do
        {
            current++;

            if (tokens->peek<OpenParenthesis>(current))
            {
                depth++;
            }

            else if (tokens->peek<CloseParenthesis>(current))
            {
                depth--;
            }
        } while (depth > 0 && !tokens->peek(current)->eof());

        if (tokens->peek<Equals>(++current))
        {
            if (tokens->peek<OpenCurlyBracket>(++current))
            {
                parseDefine();

                return;
            }

            throw OrganicParseException("Function definitions must begin with \"{\".", tokens->peek(current)->location);
        }
    }

    const Token* expression = parseExpression("");

    if (tokens->peek<Equals>())
    {
        const SourceLocation location = expression->location;

        if (dynamic_cast<const List*>(expression))
        {
            delete expression;

            throw OrganicParseException("Cannot assign a value to a list.", location);
        }

        if (dynamic_cast<const Call*>(expression))
        {
            delete expression;

            throw OrganicParseException("Function definitions must begin with \"{\".", location);
        }
    }

    context->addInstruction(expression);
}

const void Parser::parseAssign()
{
    if (const Constant* token = tokens->peek<Constant>())
    {
        throw OrganicParseException("Cannot assign a value to \"" + token->string() + "\", it is a constant.", token->location);
    }

    if (const Value* token = tokens->peek<Value>())
    {
        throw OrganicParseException("Cannot assign a value to a number.", token->location);
    }

    if (const String* token = tokens->peek<String>())
    {
        throw OrganicParseException("Cannot assign a value to a string.", token->location);
    }

    const UniqueToken<Identifier> name = tokens->take<Identifier>();

    tokens->drop();

    context->checkNameConflicts(name.get());

    context = new ParserContext(context, ContextType::Assign, name->string(), {});

    const Token* expression = parseExpression(" after \"=\"");

    const ParserContext* inner = context;

    context = context->parent;

    delete inner;

    context->addVariable(name.get(), expression);
}

const void Parser::parseDefine()
{
    const UniqueToken<Identifier> name = tokens->take<Identifier>();

    std::vector<UniqueToken<InputDef>> inputs;

    if (tokens->peek<CloseParenthesis>(1))
    {
        tokens->drop(2);
    }

    else
    {
        do
        {
            tokens->drop();

            const UniqueToken<Identifier> input = tokens->require<Identifier>(inputs.empty() ? "input name after \"(\"" : "input name after \",\"");

            for (const UniqueToken<InputDef>& def : inputs)
            {
                if (input->string() == def->string())
                {
                    throw OrganicParseException("Input \"" + input->string() + "\" specified more than once for function \"" + name->string() + "\".", input->location);
                }
            }

            tokens->expect<Colon>("\":\" after input name");

            inputs.emplace_back(new InputDef(input->location, SharedToken(parseExpression(" after \":\""))));
        } while (tokens->peek<Comma>());

        tokens->expect<CloseParenthesis>("\",\" or \")\"");
    }

    if (libraryFunctions.count(name->string()) || name->string() == "include")
    {
        throw OrganicParseException("A function already exists with the name \"" + name->string() + "\".", name->location);
    }

    context->checkNameConflicts(name.get());

    if (context->checkRecursive(ContextType::Define, name.get()))
    {
        throw OrganicParseException("Redefining a function in its own definition is not allowed.", name->location);
    }

    context = new ParserContext(context, ContextType::Define, name->string(), inputs);

    tokens->drop(2);

    while (!tokens->peek()->eof() && !tokens->peek<CloseCurlyBracket>())
    {
        parseInstruction();
    }

    tokens->expect<CloseCurlyBracket>("\"}\" at end of function definition");

    context->checkUsage();

    const Program* program = context->buildProgram(source);

    const ParserContext* inner = context;

    context = context->parent;

    delete inner;

    context->addFunction(name.get(), inputs, program);
}

const Token* Parser::parseExpression(const std::string& errorContext)
{
    if (tokens->peek<OpenSquareBracket>())
    {
        return parseList();
    }

    return parseTerms(errorContext);
}

const List* Parser::parseList()
{
    const SourceLocation location = tokens->peek()->location;

    if (tokens->peek<CloseSquareBracket>(1))
    {
        throw OrganicParseException("Empty lists are not allowed.", location);
    }

    std::vector<UniqueToken<>> items;

    do
    {
        tokens->drop();

        items.push_back(UniqueToken<>(parseExpression(items.empty() ? " after \"[\"" : " after \",\"")));
    } while (tokens->peek<Comma>());

    tokens->expect<CloseSquareBracket>("\"]\" at end of list");

    std::vector<const Token*> owned;

    for (UniqueToken<>& token : items)
    {
        owned.push_back(token.release());
    }

    return new List(location, owned);
}

const Token* Parser::parseTerms(const std::string& errorContext)
{
    const SourceLocation start = tokens->peek()->location;

    std::vector<UniqueToken<>> terms;

    while (true)
    {
        if (const OpenParenthesis* open = tokens->peek<OpenParenthesis>())
        {
            const SourceLocation location = open->location;

            tokens->drop();

            terms.push_back(UniqueToken<>(new ParenthesizedExpression(location, parseTerms(errorContext))));

            tokens->expect<CloseParenthesis>("\")\"");
        }

        else
        {
            terms.push_back(parseTerm(terms.empty() ? errorContext : (" after \"" + terms.back()->string() + "\"")));
        }

        if (tokens->peek<Operator>())
        {
            terms.push_back(tokens->take());
        }

        else
        {
            break;
        }
    }

    return collapseTerms(start, terms, 0, terms.size());
}

const Token* Parser::collapseTerms(const SourceLocation& location, std::vector<UniqueToken<>>& terms, const size_t start, const size_t end) const
{
    if (end <= start)
    {
        throw OrganicParseException("Invalid expression.", location);
    }

    if (end - start == 1)
    {
        return terms[start].release();
    }

    for (size_t i = end - 2; i > 0; i--)
    {
        const Token* left = collapseTerms(location, terms, start, i);
        const Token* right = collapseTerms(location, terms, i + 1, end);

        if (dynamic_cast<const DoubleEquals*>(terms[i].get()))
        {
            return new EqualAlias(left, right);
        }

        if (dynamic_cast<const Less*>(terms[i].get()))
        {
            return new LessAlias(left, right);
        }

        if (dynamic_cast<const Greater*>(terms[i].get()))
        {
            return new GreaterAlias(left, right);
        }

        if (dynamic_cast<const LessEqual*>(terms[i].get()))
        {
            return new LessEqualAlias(left, right);
        }

        if (dynamic_cast<const GreaterEqual*>(terms[i].get()))
        {
            return new GreaterEqualAlias(left, right);
        }

        if (dynamic_cast<const Power*>(terms[i].get()))
        {
            return new PowerAlias(left, right);
        }

        if (dynamic_cast<const Multiply*>(terms[i].get()))
        {
            return new MultiplyAlias(left, right);
        }

        if (dynamic_cast<const Divide*>(terms[i].get()))
        {
            return new DivideAlias(left, right);
        }

        if (dynamic_cast<const Add*>(terms[i].get()))
        {
            return new AddAlias(left, right);
        }

        if (dynamic_cast<const Subtract*>(terms[i].get()))
        {
            return new SubtractAlias(left, right);
        }
    }

    throw OrganicParseException("Invalid expression.", location);
}

UniqueToken<> Parser::parseTerm(const std::string& errorContext)
{
    if (tokens->peek<Value>() || tokens->peek<Constant>() || tokens->peek<String>())
    {
        return tokens->take();
    }

    if (const Identifier* name = tokens->peek<Identifier>())
    {
        if (tokens->peek<OpenParenthesis>(1))
        {
            return parseCall();
        }

        if (context->checkRecursive(ContextType::Assign, name))
        {
            throw OrganicParseException("Using a variable in its own definition is not allowed.", name->location);
        }

        const Identifier* value = context->findIdentifier(name);

        tokens->drop();

        return UniqueToken<>(value);
    }

    throw OrganicTokenException(tokens->peek(), "value" + errorContext);
}

UniqueToken<Call> Parser::parseCall()
{
    const UniqueToken<Identifier> name = tokens->take<Identifier>();

    if (name->string() == "include")
    {
        throw OrganicParseException("Includes must come before all other instructions.", name->location);
    }

    if (context->checkRecursive(ContextType::Define, name.get()))
    {
        throw OrganicParseException("Calling a function in its own definition is not allowed.", name->location);
    }

    std::vector<UniqueToken<Argument>> arguments;

    if (tokens->peek<CloseParenthesis>(1))
    {
        tokens->drop(2);
    }

    else
    {
        do
        {
            tokens->drop();

            UniqueToken<Argument> argument = parseArgument(arguments.empty() ? " after \"(\"" : " after \",\"");

            for (const UniqueToken<Argument>& arg : arguments)
            {
                if (arg->name == argument->name)
                {
                    throw OrganicParseException("Input \"" + argument->name + "\" specified more than once for function \"" + name->string() + "\".", argument->location);
                }
            }

            arguments.push_back(std::move(argument));
        } while (tokens->peek<Comma>());

        tokens->expect<CloseParenthesis>("\")\" after input value");
    }

    std::vector<const Argument*> owned;

    for (UniqueToken<Argument>& argument : arguments)
    {
        owned.push_back(argument.release());
    }

    ArgumentList* argumentList = new ArgumentList(name->location, owned, name->string());

    if (libraryFunctions.count(name->string()))
    {
        return libraryFunctions[name->string()](name->location, argumentList);
    }

    try
    {
        return UniqueToken<Call>(new CallUser(name->location, argumentList, context->findFunction(name.get())));
    }

    catch (const OrganicException& e)
    {
        delete argumentList;

        throw;
    }
}

UniqueToken<Argument> Parser::parseArgument(const std::string& errorContext)
{
    const UniqueToken<Identifier> name = tokens->require<Identifier>("input name" + errorContext);

    tokens->expect<Colon>("\":\" after input name");

    return UniqueToken<Argument>(new Argument(name->location, name->string(), SharedToken(parseExpression(" after \":\""))));
}

}
