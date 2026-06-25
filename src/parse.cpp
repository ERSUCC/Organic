#include "../include/parse.h"

namespace Parser {

struct TokenException : public OrganicParseException
{
    TokenException(const TokenListNode* node, const std::string& expected) :
        OrganicParseException(getMessage(node, expected), node->token->location), node(node), expected(expected) {}

    const TokenListNode* node;

    const std::string expected;

private:
    static std::string getMessage(const TokenListNode* node, const std::string& expected)
    {
        if (node->end)
        {
            return "Expected " + expected + ".";
        }

        return "Expected " + expected + ", but received \"" + node->token->string() + "\".";
    }

};

#define CALL(func) [](const SourceLocation& location, ArgumentList* arguments) { return new func(location, arguments); }

static std::unordered_map<std::string, std::function<Call* (const SourceLocation&, ArgumentList*)>> libraryFunctions =
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

ParserContext::ParserContext(ParserContext* parent, const std::string name, const std::vector<InputDef*>& inputs) :
    parent(parent), name(name)
{
    for (InputDef* input : inputs)
    {
        this->inputs[input->string()] = input;
    }
}

VariableDef* ParserContext::addVariable(const Identifier* token, Token* value)
{
    checkNameConflicts(token);

    VariableDef* variable = new VariableDef(token->location, value);

    variables[token->string()] = variable;

    return variable;
}

FunctionDef* ParserContext::addFunction(const Identifier* token, const std::vector<InputDef*>& inputs, const std::vector<Token*>& instructions)
{
    if (checkRecursive(token))
    {
        throw OrganicParseException("Redefining a function in its own definition is not allowed.", token->location);
    }

    checkNameConflicts(token);

    FunctionDef* function = new FunctionDef(token->location, inputs, instructions);

    functions[token->string()] = function;

    return function;
}

Identifier* ParserContext::findIdentifier(const Identifier* token)
{
    if (inputs.count(token->string()))
    {
        return new InputRef(token->location, inputs[token->string()]);
    }

    if (variables.count(token->string()))
    {
        return new VariableRef(token->location, variables[token->string()]);
    }

    if (functions.count(token->string()))
    {
        return new FunctionRef(token->location, functions[token->string()]);
    }

    if (parent)
    {
        if (Identifier* identifier = parent->findIdentifier(token))
        {
            return identifier;
        }
    }

    throw OrganicParseException("No variable, input, or function exists with the name \"" + token->string() + "\".", token->location);
}

FunctionRef* ParserContext::findFunction(const Identifier* token)
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
        if (FunctionRef* function = parent->findFunction(token))
        {
            return function;
        }
    }

    throw OrganicParseException("No function exists with the name \"" + token->string() + "\".", token->location);
}

void ParserContext::merge(const ParserContext* context)
{
    for (const std::pair<std::string, InputDef*> input : context->inputs)
    {
        checkNameConflicts(input.second);

        inputs[input.first] = input.second;
    }

    for (const std::pair<std::string, VariableDef*> variable : context->variables)
    {
        checkNameConflicts(variable.second);

        variables[variable.first] = variable.second;
    }

    for (const std::pair<std::string, FunctionDef*> function : context->functions)
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

Program* Parser::parseSource(const Path* path)
{
    const FileProvider* source = FileProvider::create(path);

    if (!source)
    {
        throw OrganicFileException("Could not read \"" + path->string() + "\".");
    }

    std::unordered_set<const Path*, Path::Hash, Path::Equals> includedPaths = { path };

    return (new Parser(source, new ParserContext(nullptr, "", {}), includedPaths))->parse();
}

Program* Parser::parseSource(const std::string& source)
{
    std::unordered_set<const Path*, Path::Hash, Path::Equals> includedPaths;

    return (new Parser(new SourceProvider(source), new ParserContext(nullptr, "", {}), includedPaths))->parse();
}

Parser::Parser(const SourceProvider* source, ParserContext* context, std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths) :
    source(source), context(context), includedPaths(includedPaths) {}

Program* Parser::parse()
{
    tokens = (new Tokenizer(source))->tokenize();

    TokenListNode* current = tokens->head->next;

    while (current->token->string() == "include" && current->next->getToken<OpenParenthesis>())
    {
        current = parseInclude(current);
    }

    while (!current->end)
    {
        current = parseInstruction(current);
    }

    std::vector<Token*> instructions;

    current = tokens->head->next;

    while (!current->end)
    {
        instructions.push_back(current->token);

        current = current->next;
    }

    return new Program(SourceLocation(source, 0, source->length()), instructions);
}

TokenListNode* Parser::parseInclude(TokenListNode* start)
{
    const SourceLocation location = start->token->location;

    TokenListNode* current = start->next->next;

    const String* str = current->getToken<String>();

    if (!str)
    {
        throw TokenException(current, "file path");
    }

    current = current->next;

    if (!current->getToken<CloseParenthesis>())
    {
        throw TokenException(current, "\")\"");
    }

    current = current->next;

    const std::filesystem::path file = Path::formatPath(str->str);

    if (file.empty())
    {
        Utils::includeWarning("This include does not specify a source file, it will have no effect.", location);

        return tokens->stitch(start, current);
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

        return tokens->stitch(start, current);
    }

    if (includedPaths.count(includePath))
    {
        Utils::includeWarning("Source file \"" + includePath->string() + "\" has already been included, this include will be ignored.", location);

        return tokens->stitch(start, current);
    }

    includedPaths.insert(includePath);

    const FileProvider* includeSource = FileProvider::create(includePath);

    ParserContext* includeContext = new ParserContext(nullptr, "", {});

    Program* program = (new Parser(includeSource, includeContext, includedPaths))->parse();

    context->merge(includeContext);

    return tokens->patch(start, current, program);
}

TokenListNode* Parser::parseInstruction(TokenListNode* start)
{
    if (start->next->getToken<Equals>())
    {
        return parseAssign(start);
    }

    if (start->next->getToken<OpenParenthesis>())
    {
        TokenListNode* current = start;

        size_t depth = 0;

        do
        {
            current = current->next;

            if (current->getToken<OpenParenthesis>())
            {
                depth++;
            }

            else if (current->getToken<CloseParenthesis>())
            {
                depth--;
            }
        } while (depth > 0 && !current->end);

        if (current->next->getToken<Equals>())
        {
            if (current->next->next->getToken<OpenCurlyBracket>())
            {
                return parseDefine(start);
            }

            throw OrganicParseException("Function definitions must begin with \"{\".", current->next->next->token->location);
        }
    }

    TokenListNode* next = parseExpression(start);

    if (next->getToken<Equals>())
    {
        if (next->prev->getToken<List>())
        {
            throw OrganicParseException("Cannot assign a value to a list.", next->prev->token->location);
        }

        if (next->prev->getToken<Call>())
        {
            throw OrganicParseException("Function definitions must begin with \"{\".", next->token->location);
        }
    }

    return next;
}

TokenListNode* Parser::parseDefine(TokenListNode* start)
{
    TokenListNode* current = start->next->next;

    std::vector<InputDef*> inputs;

    if (!current->getToken<CloseParenthesis>())
    {
        current = current->prev;

        do
        {
            current = current->next;

            const Identifier* input = current->getToken<Identifier>();

            if (!input)
            {
                throw TokenException(current, "input name");
            }

            if (std::find_if(inputs.begin(), inputs.end(), [=](const InputDef* def) { return input->string() == def->string(); }) != inputs.end())
            {
                throw OrganicParseException("Input \"" + input->string() + "\" specified more than once for function \"" + start->token->string() + "\".", input->location);
            }

            current = current->next;

            if (!current->getToken<Colon>())
            {
                throw TokenException(current, "\":\" after input name");
            }

            current = parseExpression(current->next);

            inputs.push_back(new InputDef(input->location, current->prev->token));
        } while (current->getToken<Comma>());

        if (!current->getToken<CloseParenthesis>())
        {
            throw TokenException(current, "\",\" or \")\"");
        }
    }

    const Identifier* name = start->getToken<Identifier>();

    if (libraryFunctions.count(name->string()) || name->string() == "include")
    {
        throw OrganicParseException("A function already exists with the name \"" + name->string() + "\".", name->location);
    }

    context = new ParserContext(context, name->string(), inputs);

    current = current->next->next;

    const OpenCurlyBracket* open = current->getToken<OpenCurlyBracket>();

    current = current->next;

    std::vector<Token*> instructions;

    while (!current->end && !current->getToken<CloseCurlyBracket>())
    {
        current = parseInstruction(current);

        instructions.push_back(current->prev->token);
    }

    if (!current->getToken<CloseCurlyBracket>())
    {
        throw TokenException(current, "\"}\" at end of function definition");
    }

    if (instructions.empty())
    {
        throw OrganicParseException("The function \"" + name->string() + "\" does not return a value.", name->location);
    }

    context = context->parent;

    FunctionDef* function = context->addFunction(name, inputs, instructions);

    return tokens->patch(start, current->next, new Define(function->location, function));
}

TokenListNode* Parser::parseAssign(TokenListNode* start)
{
    TokenListNode* current = start;

    if (current->getToken<Constant>())
    {
        throw OrganicParseException("Cannot assign a value to \"" + current->token->string() + "\", it is a constant.", current->token->location);
    }

    if (current->getToken<Value>())
    {
        throw OrganicParseException("Cannot assign a value to a number.", current->token->location);
    }

    if (current->getToken<String>())
    {
        throw OrganicParseException("Cannot assign a value to a string.", current->token->location);
    }

    const Identifier* name = current->getToken<Identifier>();

    try
    {
        current = parseExpression(current->next->next);
    }

    catch (const TokenException& e)
    {
        throw TokenException(e.node, "value after \"=\"");
    }

    VariableDef* variable = context->addVariable(name, current->prev->token);

    return tokens->patch(start, current, new Assign(variable->location, variable));
}

TokenListNode* Parser::parseCall(TokenListNode* start)
{
    const Identifier* name = start->getToken<Identifier>();

    if (name->string() == "include")
    {
        throw OrganicIncludeException("Includes must come before all other instructions.", name->location);
    }

    TokenListNode* current = start->next->next;

    std::vector<Argument*> arguments;

    if (!current->getToken<CloseParenthesis>())
    {
        current = current->prev;

        do
        {
            try
            {
                current = parseArgument(current->next);
            }

            catch (const TokenException& e)
            {
                if (e.expected == "input name")
                {
                    if (arguments.empty())
                    {
                        throw TokenException(e.node, "input name after \"(\"");
                    }

                    throw TokenException(e.node, "input name after \",\"");
                }

                throw e;
            }

            Argument* argument = current->prev->getToken<Argument>();

            for (const Argument* arg : arguments)
            {
                if (arg->name == argument->name)
                {
                    throw OrganicParseException("Input \"" + argument->name + "\" specified more than once for function \"" + name->string() + "\".", argument->location);
                }
            }

            arguments.push_back(argument);
        } while (current->getToken<Comma>());

        if (!current->getToken<CloseParenthesis>())
        {
            throw TokenException(current, "\")\" after input value");
        }
    }

    current = current->next;

    ArgumentList* argumentList = new ArgumentList(name->location, arguments, name->string());

    if (libraryFunctions.count(name->string()))
    {
        return tokens->patch(start, current, libraryFunctions[name->string()](name->location, argumentList));
    }

    return tokens->patch(start, current, new CallUser(name->location, argumentList, context->findFunction(name)));
}

TokenListNode* Parser::parseArgument(TokenListNode* start)
{
    TokenListNode* current = start;

    const Identifier* name = current->getToken<Identifier>();

    if (!name)
    {
        throw TokenException(current, "input name");
    }

    current = current->next;

    if (!current->getToken<Colon>())
    {
        throw TokenException(current, "\":\" after input name");
    }

    try
    {
        current = parseExpression(current->next);
    }

    catch (const TokenException& e)
    {
        throw TokenException(e.node, "input value after \":\"");
    }

    return tokens->patch(start, current, new Argument(name->location, name->string(), current->prev->token));
}

TokenListNode* Parser::parseExpression(TokenListNode* start)
{
    if (start->getToken<OpenSquareBracket>())
    {
        return parseList(start);
    }

    return parseTerms(start);
}

TokenListNode* Parser::parseList(TokenListNode* start)
{
    TokenListNode* current = start;

    if (current->next->getToken<CloseSquareBracket>())
    {
        throw OrganicParseException("Empty lists are not allowed.", start->token->location);
    }

    std::vector<Token*> items;

    do
    {
        try
        {
            current = parseExpression(current->next);
        }

        catch (const TokenException& e)
        {
            if (e.expected == "value")
            {
                if (items.empty())
                {
                    throw TokenException(e.node, "value after \"[\"");
                }

                throw TokenException(e.node, "value after \",\"");
            }

            throw e;
        }

        items.push_back(current->prev->token);
    } while (current->getToken<Comma>());

    if (!current->getToken<CloseSquareBracket>())
    {
        throw TokenException(current, "\"]\" at end of list");
    }

    return tokens->patch(start, current->next, new List(start->token->location, items));
}

TokenListNode* Parser::parseTerms(TokenListNode* start)
{
    TokenListNode* current = start;

    while (true)
    {
        if (current->getToken<OpenParenthesis>())
        {
            TokenListNode* first = current;

            current = parseTerms(current->next);

            if (!current->getToken<CloseParenthesis>())
            {
                throw TokenException(current, "\")\"");
            }

            current = tokens->patch(first, current->next, new ParenthesizedExpression(first->token->location, current->prev->token));
        }

        else
        {
            current = parseTerm(current);
        }

        if (current->getToken<Operator>())
        {
            current = current->next;
        }

        else
        {
            break;
        }
    }

    TokenListNode* pStart = start->prev;

    TokenListNode* term = pStart->next->next;

    bool comparison = false;

    while (!term->next->end && term != current)
    {
        const SourceLocation location = term->prev->token->location;

        if (term->getToken<DoubleEquals>())
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", term->token->location);
            }

            comparison = true;

            term = tokens->patch(term->prev, term->next->next, new EqualAlias(location, term->prev->token, term->next->token));
        }

        else if (term->getToken<Less>())
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", term->token->location);
            }

            comparison = true;

            term = tokens->patch(term->prev, term->next->next, new LessAlias(location, term->prev->token, term->next->token));
        }

        else if (term->getToken<Greater>())
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", term->token->location);
            }

            comparison = true;

            term = tokens->patch(term->prev, term->next->next, new GreaterAlias(location, term->prev->token, term->next->token));
        }

        else if (term->getToken<LessEqual>())
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", term->token->location);
            }

            comparison = true;

            term = tokens->patch(term->prev, term->next->next, new LessEqualAlias(location, term->prev->token, term->next->token));
        }

        else if (term->getToken<GreaterEqual>())
        {
            if (comparison)
            {
                throw OrganicParseException("Chaining comparison operators is not allowed.", term->token->location);
            }

            comparison = true;

            term = tokens->patch(term->prev, term->next->next, new GreaterEqualAlias(location, term->prev->token, term->next->token));
        }

        else
        {
            term = term->next;
        }
    }

    term = pStart->next->next;

    while (!term->next->end && term != current)
    {
        if (term->getToken<Power>())
        {
            term = tokens->patch(term->prev, term->next->next, new PowerAlias(term->prev->token->location, term->prev->token, term->next->token));
        }

        else
        {
            term = term->next;
        }
    }

    term = pStart->next->next;

    while (!term->next->end && term != current)
    {
        const SourceLocation location = term->prev->token->location;

        if (term->getToken<Multiply>())
        {
            term = tokens->patch(term->prev, term->next->next, new MultiplyAlias(location, term->prev->token, term->next->token));
        }

        else if (term->getToken<Divide>())
        {
            term = tokens->patch(term->prev, term->next->next, new DivideAlias(location, term->prev->token, term->next->token));
        }

        else
        {
            term = term->next;
        }
    }

    term = pStart->next->next;

    while (!term->next->end && term != current)
    {
        const SourceLocation location = term->prev->token->location;

        if (term->getToken<Add>())
        {
            term = tokens->patch(term->prev, term->next->next, new AddAlias(location, term->prev->token, term->next->token));
        }

        else if (term->getToken<Subtract>())
        {
            term = tokens->patch(term->prev, term->next->next, new SubtractAlias(location, term->prev->token, term->next->token));
        }

        else
        {
            term = term->next;
        }
    }

    if (pStart->next->next != current)
    {
        throw OrganicParseException("Invalid expression.", pStart->next->token->location);
    }

    return current;
}

TokenListNode* Parser::parseTerm(TokenListNode* start)
{
    if (start->getToken<Value>() || start->getToken<Constant>() || start->getToken<String>())
    {
        return start->next;
    }

    if (const Identifier* name = start->getToken<Identifier>())
    {
        if (start->next->getToken<OpenParenthesis>())
        {
            return parseCall(start);
        }

        start->token = context->findIdentifier(name);

        return start->next;
    }

    throw TokenException(start, "value");
}

}
