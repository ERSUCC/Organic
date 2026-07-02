#include "../include/parse.h"

namespace Parser {

#define CALL(func) [](const SourceLocation& location, ArgumentList* arguments) { return new func(location, arguments); }

static std::unordered_map<std::string, std::function<const Call* (const SourceLocation&, ArgumentList*)>> libraryFunctions =
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

ParserContext::ParserContext(ParserContext* parent, const std::string name, const std::vector<UniqueToken<InputDef>>& inputs) :
    parent(parent), name(name)
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

const FunctionRef* ParserContext::findFunction(const Identifier* token)
{
    if (checkRecursive(token))
    {
        throw OrganicParseException("Calling a function in its own definition is not allowed.", token->location);
    }

    if (functions.count(token->string()))
    {
        return new FunctionRef(token->location, functions[token->string()]);
    }

    if (parent)
    {
        if (const FunctionRef* function = parent->findFunction(token))
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
}

bool ParserContext::checkRecursive(const Identifier* token) const
{
    return name == token->string() || (parent && parent->checkRecursive(token));
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
    ParserContext* context = new ParserContext(nullptr, "", {});

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

    const Token* expression = parseExpression();

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

    try
    {
        context->addVariable(name.get(), parseExpression());
    }

    catch (const OrganicTokenException& e)
    {
        throw OrganicTokenException(e.token, "value after \"=\"");
    }
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

            const UniqueToken<Identifier> input = tokens->require<Identifier>("input name");

            for (const UniqueToken<InputDef>& def : inputs)
            {
                if (input->string() == def->string())
                {
                    throw OrganicParseException("Input \"" + input->string() + "\" specified more than once for function \"" + name->string() + "\".", input->location);
                }
            }

            tokens->expect<Colon>("\":\" after input name");

            inputs.emplace_back(new InputDef(input->location, SharedToken(parseExpression())));
        } while (tokens->peek<Comma>());

        tokens->expect<CloseParenthesis>("\",\" or \")\"");
    }

    if (libraryFunctions.count(name->string()) || name->string() == "include")
    {
        throw OrganicParseException("A function already exists with the name \"" + name->string() + "\".", name->location);
    }

    context->checkNameConflicts(name.get());

    if (context->checkRecursive(name.get()))
    {
        throw OrganicParseException("Redefining a function in its own definition is not allowed.", name->location);
    }

    context = new ParserContext(context, name->string(), inputs);

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

const Call* Parser::parseCall()
{
    const UniqueToken<Identifier> name = tokens->take<Identifier>();

    if (name->string() == "include")
    {
        throw OrganicParseException("Includes must come before all other instructions.", name->location);
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

            try
            {
                UniqueToken<Argument> argument = UniqueToken<Argument>(parseArgument());

                for (const UniqueToken<Argument>& arg : arguments)
                {
                    if (arg->name == argument->name)
                    {
                        throw OrganicParseException("Input \"" + argument->name + "\" specified more than once for function \"" + name->string() + "\".", argument->location);
                    }
                }

                arguments.push_back(std::move(argument));
            }

            catch (const OrganicTokenException& e)
            {
                if (e.expected == "input name")
                {
                    if (arguments.empty())
                    {
                        throw OrganicTokenException(e.token, "input name after \"(\"");
                    }

                    throw OrganicTokenException(e.token, "input name after \",\"");
                }

                throw;
            }
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

    return new CallUser(name->location, argumentList, context->findFunction(name.get()));
}

const Argument* Parser::parseArgument()
{
    const UniqueToken<Identifier> name = tokens->require<Identifier>("input name");

    tokens->expect<Colon>("\":\" after input name");

    try
    {
        return new Argument(name->location, name->string(), SharedToken(parseExpression()));
    }

    catch (const OrganicTokenException& e)
    {
        throw OrganicTokenException(e.token, "input value after \":\"");
    }
}

const Token* Parser::parseExpression()
{
    if (tokens->peek<OpenSquareBracket>())
    {
        return parseList();
    }

    return parseTerms();
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

        try
        {
            items.push_back(UniqueToken<>(parseExpression()));
        }

        catch (const OrganicTokenException& e)
        {
            if (e.expected == "value")
            {
                if (items.empty())
                {
                    throw OrganicTokenException(e.token, "value after \"[\"");
                }

                throw OrganicTokenException(e.token, "value after \",\"");
            }

            throw e;
        }
    } while (tokens->peek<Comma>());

    tokens->expect<CloseSquareBracket>("\"]\" at end of list");

    std::vector<const Token*> owned;

    for (UniqueToken<>& token : items)
    {
        owned.push_back(token.release());
    }

    return new List(location, owned);
}

const Token* Parser::parseTerms()
{
    const SourceLocation location = tokens->peek()->location;

    std::vector<const Token*> terms;

    while (true)
    {
        if (tokens->peek<OpenParenthesis>())
        {
            tokens->drop();

            terms.push_back(new ParenthesizedExpression(location, parseTerms()));

            tokens->expect<CloseParenthesis>("\")\"");
        }

        else
        {
            terms.push_back(parseTerm());
        }

        if (tokens->peek<Operator>())
        {
            terms.push_back(tokens->take().release());
        }

        else
        {
            break;
        }
    }

    size_t i = 1;

    bool comparison = false;

    while (i < terms.size() - 1)
    {
        if (dynamic_cast<const DoubleEquals*>(terms[i]))
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
            }

            comparison = true;

            delete terms[i];

            terms[i - 1] = new EqualAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else if (dynamic_cast<const Less*>(terms[i]))
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
            }

            comparison = true;

            delete terms[i];

            terms[i - 1] = new LessAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else if (dynamic_cast<const Greater*>(terms[i]))
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
            }

            comparison = true;

            delete terms[i];

            terms[i - 1] = new GreaterAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else if (dynamic_cast<const LessEqual*>(terms[i]))
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
            }

            comparison = true;

            delete terms[i];

            terms[i - 1] = new LessEqualAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else if (dynamic_cast<const GreaterEqual*>(terms[i]))
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
            }

            comparison = true;

            delete terms[i];

            terms[i - 1] = new GreaterEqualAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else
        {
            i++;
        }
    }

    i = 1;

    while (i < terms.size() - 1)
    {
        if (dynamic_cast<const Power*>(terms[i]))
        {
            delete terms[i];

            terms[i - 1] = new PowerAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else
        {
            i++;
        }
    }

    i = 1;

    while (i < terms.size() - 1)
    {
        if (dynamic_cast<const Multiply*>(terms[i]))
        {
            delete terms[i];

            terms[i - 1] = new MultiplyAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else if (dynamic_cast<const Divide*>(terms[i]))
        {
            delete terms[i];

            terms[i - 1] = new DivideAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else
        {
            i++;
        }
    }

    i = 1;

    while (i < terms.size() - 1)
    {
        if (dynamic_cast<const Add*>(terms[i]))
        {
            delete terms[i];

            terms[i - 1] = new AddAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else if (dynamic_cast<const Subtract*>(terms[i]))
        {
            delete terms[i];

            terms[i - 1] = new SubtractAlias(terms[i - 1]->location, terms[i - 1], terms[i + 1]);

            terms.erase(terms.begin() + i, terms.begin() + i + 2);
        }

        else
        {
            i++;
        }
    }

    if (terms.size() != 1)
    {
        throw OrganicParseException("Invalid expression.", location);
    }

    return terms[0];
}

const Token* Parser::parseTerm()
{
    if (tokens->peek<Value>() || tokens->peek<Constant>() || tokens->peek<String>())
    {
        return tokens->take().release();
    }

    if (const Identifier* name = tokens->peek<Identifier>())
    {
        if (tokens->peek<OpenParenthesis>(1))
        {
            return parseCall();
        }

        const Identifier* value = context->findIdentifier(name);

        tokens->drop();

        return value;
    }

    throw OrganicTokenException(tokens->peek(), "value");
}

}
