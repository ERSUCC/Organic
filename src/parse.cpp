#include "../include/parse.h"

namespace Parser
{
    ParserContext::ParserContext(ParserContext* parent, const std::string name, const std::vector<InputDef*>& inputs) :
        parent(parent), name(name)
    {
        for (InputDef* input : inputs)
        {
            this->inputs[input->str] = input;
        }
    }

    VariableDef* ParserContext::addVariable(const Identifier* token)
    {
        checkNameConflicts(token);

        VariableDef* variable = new VariableDef(token->location, token->str);

        variables[token->str] = variable;

        return variable;
    }

    FunctionDef* ParserContext::addFunction(const Identifier* token, const std::vector<InputDef*>& inputs)
    {
        if (checkRecursive(token))
        {
            throw OrganicParseException("Redefining a function in its own definition is not allowed.", token->location);
        }

        checkNameConflicts(token);

        FunctionDef* lambda = new FunctionDef(token->location, token->str, inputs);

        functions[token->str] = lambda;

        return lambda;
    }

    Identifier* ParserContext::findIdentifier(const Identifier* token)
    {
        if (inputs.count(token->str))
        {
            return new InputRef(token->location, inputs[token->str]);
        }

        if (variables.count(token->str))
        {
            return new VariableRef(token->location, variables[token->str]);
        }

        if (functions.count(token->str))
        {
            return new FunctionRef(token->location, functions[token->str]);
        }

        if (parent)
        {
            if (Identifier* identifier = parent->findIdentifier(token))
            {
                return identifier;
            }
        }

        throw OrganicParseException("No variable, input, or function exists with the name \"" + token->str + "\".", token->location);
    }

    FunctionRef* ParserContext::findFunction(const Identifier* token)
    {
        if (checkRecursive(token))
        {
            throw OrganicParseException("Calling a function in its own definition is not allowed.", token->location);
        }

        if (functions.count(token->str))
        {
            return new FunctionRef(token->location, functions[token->str]);
        }

        if (parent)
        {
            if (FunctionRef* function = parent->findFunction(token))
            {
                return function;
            }
        }

        throw OrganicParseException("No function exists with the name \"" + token->str + "\".", token->location);
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
        if (inputs.count(token->str))
        {
            throw OrganicParseException("An input already exists with the name \"" + token->str + "\".", token->location);
        }

        if (variables.count(token->str))
        {
            throw OrganicParseException("A variable already exists with the name \"" + token->str + "\".", token->location);
        }

        if (functions.count(token->str))
        {
            throw OrganicParseException("A function already exists with the name \"" + token->str + "\".", token->location);
        }
    }

    bool ParserContext::checkRecursive(const Identifier* token) const
    {
        return name == token->str || (parent && parent->checkRecursive(token));
    }

    Parser::Parser(const Path* path, ParserContext* context, std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths) :
        path(path), context(context), includedPaths(includedPaths) {}

    Program* Parser::parse()
    {
        tokens = (new Tokenizer(path))->tokenize();

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

        return new Program(SourceLocation(path, 1, 1), instructions);
    }

    void Parser::tokenError(const Token* token, const std::string expected) const
    {
        throw OrganicParseException("Expected " + expected + ", received \"" + token->string() + "\".", token->location);
    }

    TokenListNode* Parser::parseInclude(TokenListNode* start)
    {
        const SourceLocation location = start->token->location;

        TokenListNode* current = start->next->next;

        const String* str = current->getToken<String>();

        if (!str)
        {
            tokenError(current->token, "file path");
        }

        current = current->next;

        if (!current->getToken<CloseParenthesis>())
        {
            tokenError(current->token, "\")\"");
        }

        current = current->next;

        const std::filesystem::path file = Path::formatPath(str->str);

        if (file.empty())
        {
            Utils::includeWarning("This include does not specify a source file, it will have no effect.", location);

            return tokens->stitch(start, current);
        }

        const Path* includePath = Path::beside(file, path);

        if (!includePath->exists())
        {
            throw OrganicIncludeException("Source file \"" + includePath->string() + "\" does not exist.", location);
        }

        if (!includePath->isFile())
        {
            throw OrganicIncludeException("\"" + includePath->string() + "\" is not a file.", location);
        }

        if (path->string() == includePath->string())
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

        ParserContext* includeContext = new ParserContext(nullptr, "", {});

        Program* program = (new Parser(includePath, includeContext, includedPaths))->parse();

        context->merge(includeContext);

        return tokens->patch(start, current, program);
    }

    TokenListNode* Parser::parseInstruction(TokenListNode* start)
    {
        TokenListNode* current = start;

        if (current->next->getToken<Equals>())
        {
            return parseAssign(current);
        }

        if (current->next->getToken<OpenParenthesis>())
        {
            unsigned int depth = 0;

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

            current = current->next;

            if (current->getToken<Equals>())
            {
                current = current->next;

                if (current->getToken<OpenCurlyBracket>())
                {
                    return parseDefine(start);
                }

                tokenError(current->token, "\"{\"");
            }
        }

        current = parseExpression(start);

        if (AudioSource* audioSource = current->prev->getToken<AudioSource>())
        {
            current->prev->token = new Play(audioSource->location, audioSource);
        }

        return current;
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

                if (const Identifier* input = current->getToken<Identifier>())
                {
                    inputs.push_back(new InputDef(input->location, input->str));
                }

                else
                {
                    tokenError(current->token, "input name");
                }

                current = current->next;
            } while (current->getToken<Comma>());

            if (!current->getToken<CloseParenthesis>())
            {
                tokenError(current->token, "\",\" or \")\"");
            }
        }

        FunctionDef* function = context->addFunction(start->getToken<Identifier>(), inputs);

        if (function->str == "time" ||
            function->str == "hold" ||
            function->str == "lfo" ||
            function->str == "sweep" ||
            function->str == "sequence" ||
            function->str == "repeat" ||
            function->str == "random" ||
            function->str == "limit" ||
            function->str == "trigger" ||
            function->str == "if" ||
            function->str == "all" ||
            function->str == "any" ||
            function->str == "none" ||
            function->str == "sine" ||
            function->str == "square" ||
            function->str == "triangle" ||
            function->str == "saw" ||
            function->str == "noise" ||
            function->str == "sample" ||
            function->str == "delay" ||
            function->str == "include")
        {
            throw OrganicParseException("A function already exists with the name \"" + function->str + "\".", function->location);
        }

        context = new ParserContext(context, function->str, inputs);

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
            tokenError(current->token, "\"}\"");
        }

        context = context->parent;

        return tokens->patch(start, current->next, new Define(function->location, instructions, function));
    }

    TokenListNode* Parser::parseAssign(TokenListNode* start)
    {
        TokenListNode* current = start;

        const Identifier* name = current->getToken<Identifier>();

        if (!name)
        {
            tokenError(current->token, "variable name");
        }

        VariableDef* variable = context->addVariable(name);

        current = parseExpression(current->next->next);

        return tokens->patch(start, current, new Assign(variable->location, variable, current->prev->token));
    }

    TokenListNode* Parser::parseCall(TokenListNode* start)
    {
        TokenListNode* current = start;

        const Identifier* name = start->getToken<Identifier>();

        current = current->next->next;

        std::vector<Argument*> arguments;

        if (!current->getToken<CloseParenthesis>())
        {
            current = current->prev;

            do
            {
                current = parseArgument(current->next);

                Argument* argument = current->prev->getToken<Argument>();

                for (const Argument* arg : arguments)
                {
                    if (arg->name == argument->name)
                    {
                        throw OrganicParseException("Input \"" + argument->name + "\" specified more than once for function \"" + name->str + "\".", argument->location);
                    }
                }

                arguments.push_back(argument);
            } while (current->getToken<Comma>());

            if (!current->getToken<CloseParenthesis>())
            {
                tokenError(current->token, "\")\"");
            }
        }

        current = current->next;

        ArgumentList* argumentList = new ArgumentList(name->location, arguments, name->str);

        if (name->str == "time")
        {
            return tokens->patch(start, current, new Time(name->location, argumentList));
        }

        if (name->str == "hold")
        {
            return tokens->patch(start, current, new Hold(name->location, argumentList));
        }

        if (name->str == "lfo")
        {
            return tokens->patch(start, current, new LFO(name->location, argumentList));
        }

        if (name->str == "sweep")
        {
            return tokens->patch(start, current, new Sweep(name->location, argumentList));
        }

        if (name->str == "sequence")
        {
            return tokens->patch(start, current, new Sequence(name->location, argumentList));
        }

        if (name->str == "repeat")
        {
            return tokens->patch(start, current, new Repeat(name->location, argumentList));
        }

        if (name->str == "random")
        {
            return tokens->patch(start, current, new Random(name->location, argumentList));
        }

        if (name->str == "limit")
        {
            return tokens->patch(start, current, new Limit(name->location, argumentList));
        }

        if (name->str == "trigger")
        {
            return tokens->patch(start, current, new Trigger(name->location, argumentList));
        }

        if (name->str == "if")
        {
            return tokens->patch(start, current, new If(name->location, argumentList));
        }

        if (name->str == "all")
        {
            return tokens->patch(start, current, new All(name->location, argumentList));
        }

        if (name->str == "any")
        {
            return tokens->patch(start, current, new Any(name->location, argumentList));
        }

        if (name->str == "none")
        {
            return tokens->patch(start, current, new None(name->location, argumentList));
        }

        if (name->str == "min")
        {
            return tokens->patch(start, current, new Min(name->location, argumentList));
        }

        if (name->str == "max")
        {
            return tokens->patch(start, current, new Max(name->location, argumentList));
        }

        if (name->str == "round")
        {
            return tokens->patch(start, current, new Round(name->location, argumentList));
        }

        if (name->str == "sine")
        {
            return tokens->patch(start, current, new Sine(name->location, argumentList));
        }

        if (name->str == "square")
        {
            return tokens->patch(start, current, new Square(name->location, argumentList));
        }

        if (name->str == "triangle")
        {
            return tokens->patch(start, current, new Triangle(name->location, argumentList));
        }

        if (name->str == "saw")
        {
            return tokens->patch(start, current, new Saw(name->location, argumentList));
        }

        if (name->str == "noise")
        {
            return tokens->patch(start, current, new Noise(name->location, argumentList));
        }

        if (name->str == "sample")
        {
            return tokens->patch(start, current, new Sample(name->location, argumentList));
        }

        if (name->str == "oscillator")
        {
            return tokens->patch(start, current, new Oscillator(name->location, argumentList));
        }

        if (name->str == "delay")
        {
            return tokens->patch(start, current, new Delay(name->location, argumentList));
        }

        if (name->str == "include")
        {
            throw OrganicIncludeException("Includes must come before all other instructions.", name->location);
        }

        return tokens->patch(start, current, new CallUser(name->location, argumentList, context->findFunction(name)));
    }

    TokenListNode* Parser::parseArgument(TokenListNode* start)
    {
        TokenListNode* current = start;

        const Identifier* name = current->getToken<Identifier>();

        if (!name)
        {
            tokenError(current->token, "input name");
        }

        current = current->next;

        if (!current->getToken<Colon>())
        {
            tokenError(current->token, "\":\"");
        }

        current = parseExpression(current->next);

        return tokens->patch(start, current, new Argument(name->location, name->str, current->prev->token));
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
            current = parseExpression(current->next);

            items.push_back(current->prev->token);
        } while (current->getToken<Comma>());

        if (!current->getToken<CloseSquareBracket>())
        {
            tokenError(current->token, "\"]\"");
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
                    tokenError(current->token, "\")\"");
                }

                current = tokens->patch(first, current->next, new ParenthesizedExpression(first->token->location, current->token));
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

        tokenError(start->token, "expression term");

        return nullptr;
    }
}
