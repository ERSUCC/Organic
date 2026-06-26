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

ParserContext::ParserContext(ParserContext* parent, const std::string name, const std::vector<const InputDef*>& inputs) :
    parent(parent), name(name)
{
    for (const InputDef* input : inputs)
    {
        this->inputs[input->string()] = input;
    }
}

const VariableDef* ParserContext::addVariable(const Identifier* token, const Token* value)
{
    checkNameConflicts(token);

    const VariableDef* variable = new VariableDef(token->location, value);

    variables[token->string()] = variable;

    return variable;
}

const FunctionDef* ParserContext::addFunction(const Identifier* token, const std::vector<const InputDef*>& inputs, const std::vector<const Token*>& instructions)
{
    if (checkRecursive(token))
    {
        throw OrganicParseException("Redefining a function in its own definition is not allowed.", token->location);
    }

    checkNameConflicts(token);

    const FunctionDef* function = new FunctionDef(token->location, inputs, instructions);

    functions[token->string()] = function;

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

void ParserContext::merge(const ParserContext* context)
{
    for (const std::pair<std::string, const InputDef*> input : context->inputs)
    {
        checkNameConflicts(input.second);

        inputs[input.first] = input.second;
    }

    for (const std::pair<std::string, const VariableDef*> variable : context->variables)
    {
        checkNameConflicts(variable.second);

        variables[variable.first] = variable.second;
    }

    for (const std::pair<std::string, const FunctionDef*> function : context->functions)
    {
        checkNameConflicts(function.second);

        functions[function.first] = function.second;
    }
}

void ParserContext::checkNameConflicts(const Identifier* token) const
{
    if (inputs.count(token->string()))
    {
        throw OrganicParseException("An input already exists with the name \"" + token->string() + "\".", token->location);
    }

    if (variables.count(token->string()))
    {
        throw OrganicParseException("A variable already exists with the name \"" + token->string() + "\".", token->location);
    }

    if (functions.count(token->string()))
    {
        throw OrganicParseException("A function already exists with the name \"" + token->string() + "\".", token->location);
    }
}

bool ParserContext::checkRecursive(const Identifier* token) const
{
    return name == token->string() || (parent && parent->checkRecursive(token));
}

const Program* Parser::parseSource(const SourceProvider* source)
{
    std::unordered_set<const Path*, Path::Hash, Path::Equals> includedPaths = { source->path() };

    Parser* parser = new Parser(source, new ParserContext(nullptr, "", {}), includedPaths);

    const Program* program = parser->parse();

    delete parser;

    return program;
}

Parser::Parser(const SourceProvider* source, ParserContext* context, std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths) :
    source(source), context(context), includedPaths(includedPaths) {}

Parser::~Parser()
{
    delete context;
    delete tokens;
}

const Program* Parser::parse()
{
    tokens = Tokenizer::tokenize(source);

    std::vector<const Token*> instructions;

    while (tokens->peek()->string() == "include" && tokens->peek<OpenParenthesis>(1))
    {
        const Program* program = parseInclude();

        for (const Token* instruction : program->instructions)
        {
            instructions.push_back(instruction);
        }

        delete program;
    }

    while (!tokens->peek()->eof())
    {
        instructions.push_back(parseInstruction());
    }

    context->checkUsage();

    return new Program(SourceLocation(source, 0, source->length()), instructions);
}

const Program* Parser::parseInclude()
{
    const SourceLocation location = tokens->peek()->location;

    const String* str = tokens->drop(2)->require<String>("file path");

    tokens->expect<CloseParenthesis>("\")\"");

    const std::filesystem::path file = Path::formatPath(str->str);

    delete str;

    if (file.empty())
    {
        Utils::includeWarning("This include does not specify a source file, it will have no effect.", location);

        return new Program(location, {});
    }

    const Path* sourcePath = source->path();
    const Path* includePath = Path::beside(file, sourcePath);

    if (!includePath->exists())
    {
        throw OrganicIncludeException("Source file \"" + includePath->string() + "\" does not exist.", location);
    }

    if (!includePath->isFile())
    {
        throw OrganicIncludeException("\"" + includePath->string() + "\" is not a file.", location);
    }

    if (sourcePath->string() == includePath->string())
    {
        Utils::includeWarning("Source file \"" + includePath->string() + "\" is the current file, this include will be ignored.", location);

        delete sourcePath;
        delete includePath;

        return new Program(location, {});
    }

    delete sourcePath;

    if (includedPaths.count(includePath))
    {
        Utils::includeWarning("Source file \"" + includePath->string() + "\" has already been included, this include will be ignored.", location);

        delete includePath;

        return new Program(location, {});
    }

    includedPaths.insert(includePath);

    const FileProvider* includeSource = FileProvider::create(includePath);

    ParserContext* includeContext = new ParserContext(nullptr, "", {});

    Parser* parser = new Parser(includeSource, includeContext, includedPaths);

    const Program* program = parser->parse();

    context->merge(includeContext);

    delete parser;
    delete includePath;
    delete includeContext;

    return program;
}

const Token* Parser::parseInstruction()
{
    if (tokens->peek<Equals>(1))
    {
        return parseAssign();
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
                return parseDefine();
            }

            throw OrganicParseException("Function definitions must begin with \"{\".", tokens->peek(current)->location);
        }
    }

    const Token* expression = parseExpression();

    if (tokens->peek<Equals>())
    {
        if (dynamic_cast<const List*>(expression))
        {
            throw OrganicParseException("Cannot assign a value to a list.", expression->location);
        }

        if (dynamic_cast<const Call*>(expression))
        {
            throw OrganicParseException("Function definitions must begin with \"{\".", expression->location);
        }
    }

    return expression;
}

const FunctionDef* Parser::parseDefine()
{
    const Identifier* name = tokens->take<Identifier>();

    std::vector<const InputDef*> inputs;

    if (tokens->peek<CloseParenthesis>(1))
    {
        tokens->drop(2);
    }

    else
    {
        do
        {
            tokens->drop();

            const Identifier* input = tokens->require<Identifier>("input name");

            if (std::find_if(inputs.begin(), inputs.end(), [=](const InputDef* def) { return input->string() == def->string(); }) != inputs.end())
            {
                throw OrganicParseException("Input \"" + input->string() + "\" specified more than once for function \"" + name->string() + "\".", input->location);
            }

            tokens->expect<Colon>("\":\" after input name");

            inputs.push_back(new InputDef(input->location, SharedToken(parseExpression())));

            delete input;
        } while (tokens->peek<Comma>());

        tokens->expect<CloseParenthesis>("\",\" or \")\"");
    }

    if (libraryFunctions.count(name->string()) || name->string() == "include")
    {
        throw OrganicParseException("A function already exists with the name \"" + name->string() + "\".", name->location);
    }

    context = new ParserContext(context, name->string(), inputs);

    tokens->drop(2);

    std::vector<const Token*> instructions;

    while (!tokens->peek()->eof() && !tokens->peek<CloseCurlyBracket>())
    {
        instructions.push_back(parseInstruction());
    }

    tokens->expect<CloseCurlyBracket>("\"}\" at end of function definition");

    if (instructions.empty())
    {
        throw OrganicParseException("The function \"" + name->string() + "\" does not return a value.", name->location);
    }

    context->checkUsage();

    ParserContext* inner = context;

    context = context->parent;

    delete inner;

    const FunctionDef* function = context->addFunction(name, inputs, instructions);

    delete name;

    return function;
}

const VariableDef* Parser::parseAssign()
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

    try
    {
        const Identifier* name = tokens->take<Identifier>();

        tokens->drop();

        const VariableDef* variable = context->addVariable(name, parseExpression());

        delete name;

        return variable;
    }

    catch (const OrganicTokenException& e)
    {
        throw OrganicTokenException(e.token, "value after \"=\"");
    }
}

const Call* Parser::parseCall()
{
    const Identifier* name = tokens->take<Identifier>();

    if (name->string() == "include")
    {
        throw OrganicIncludeException("Includes must come before all other instructions.", name->location);
    }

    std::vector<const Argument*> arguments;

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
                const Argument* argument = parseArgument();

                for (const Argument* arg : arguments)
                {
                    if (arg->name == argument->name)
                    {
                        throw OrganicParseException("Input \"" + argument->name + "\" specified more than once for function \"" + name->string() + "\".", argument->location);
                    }
                }

                arguments.push_back(argument);
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

                throw e;
            }
        } while (tokens->peek<Comma>());

        tokens->expect<CloseParenthesis>("\")\" after input value");
    }

    ArgumentList* argumentList = new ArgumentList(name->location, arguments, name->string());

    if (libraryFunctions.count(name->string()))
    {
        const Call* call = libraryFunctions[name->string()](name->location, argumentList);

        delete name;

        return call;
    }

    const CallUser* call = new CallUser(name->location, argumentList, context->findFunction(name));

    delete name;

    return call;
}

const Argument* Parser::parseArgument()
{
    const Identifier* name = tokens->require<Identifier>("input name");

    tokens->expect<Colon>("\":\" after input name");

    try
    {
        const Argument* argument = new Argument(name->location, name->string(), SharedToken(parseExpression()));

        delete name;

        return argument;
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

    std::vector<const Token*> items;

    do
    {
        tokens->drop();

        try
        {
            items.push_back(parseExpression());
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

    return new List(location, items);
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
            terms.push_back(tokens->take());
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
        return tokens->take();
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
