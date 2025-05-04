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

        unsigned int current = 0;

        std::vector<Token*> instructions;

        while (tokenIs<Identifier>(current) && tokenIs<OpenParenthesis>(current + 1) && getToken(current)->str == "include")
        {
            Include* include = parseInclude(current);

            if (include->program)
            {
                instructions.push_back(include);
            }

            current = include->location.end;
        }

        while (current < tokens.size() - 1)
        {
            Token* token = parseInstruction(current);

            instructions.push_back(token);

            current = token->location.end;
        }

        if (instructions.empty())
        {
            return new Program(SourceLocation(path, 1, 1, 0, 0), instructions);
        }

        return new Program(SourceLocation(path, instructions[0]->location.line, instructions[0]->location.character, instructions[0]->location.start, instructions.back()->location.end), instructions);
    }

    BasicToken* Parser::getToken(const unsigned int pos) const
    {
        if (pos >= tokens.size() - 1)
        {
            throw OrganicParseException("Unexpected end of file.", tokens.back()->location);
        }

        return tokens[pos];
    }

    template <typename T> T* Parser::getToken(const unsigned int pos) const
    {
        return dynamic_cast<T*>(getToken(pos));
    }

    template <typename T> bool Parser::tokenIs(const unsigned int pos) const
    {
        if (pos >= tokens.size() - 1)
        {
            return false;
        }

        return dynamic_cast<T*>(getToken(pos));
    }

    void Parser::tokenError(const BasicToken* token, const std::string expected) const
    {
        throw OrganicParseException("Expected " + expected + ", received \"" + token->str + "\".", token->location);
    }

    Include* Parser::parseInclude(unsigned int pos) const
    {
        const BasicToken* start = getToken(pos);

        pos += 2;

        if (!tokenIs<String>(pos))
        {
            tokenError(getToken(pos), "file path");
        }

        const std::filesystem::path file = Path::formatPath(getToken<String>(pos++)->str);

        if (!tokenIs<CloseParenthesis>(pos))
        {
            tokenError(getToken(pos), "\")\"");
        }

        pos++;

        const SourceLocation location = SourceLocation(path, start->location.line, start->location.character, start->location.start, pos);

        if (file.empty())
        {
            Utils::includeWarning("This include does not specify a source file, it will have no effect.", start->location);

            return new Include(location, nullptr);
        }

        const Path* includePath = Path::beside(file, path);

        if (!includePath->exists())
        {
            throw OrganicIncludeException("Source file \"" + includePath->string() + "\" does not exist.", start->location);
        }

        if (!includePath->isFile())
        {
            throw OrganicIncludeException("\"" + includePath->string() + "\" is not a file.", start->location);
        }

        if (path->string() == includePath->string())
        {
            Utils::includeWarning("Source file \"" + includePath->string() + "\" is the current file, this include will be ignored.", start->location);

            return new Include(location, nullptr);
        }

        if (includedPaths.count(includePath))
        {
            Utils::includeWarning("Source file \"" + includePath->string() + "\" has already been included, this include will be ignored.", start->location);

            return new Include(location, nullptr);
        }

        includedPaths.insert(includePath);

        ParserContext* includeContext = new ParserContext(nullptr, "", {});

        Program* program = (new Parser(includePath, includeContext, includedPaths))->parse();

        context->merge(includeContext);

        return new Include(location, program);
    }

    Token* Parser::parseInstruction(unsigned int pos)
    {
        if (tokenIs<Equals>(pos + 1))
        {
            return parseAssign(pos);
        }

        if (tokenIs<OpenParenthesis>(pos + 1))
        {
            unsigned int current = pos;
            unsigned int depth = 0;

            do
            {
                current++;

                if (tokenIs<OpenParenthesis>(current))
                {
                    depth++;
                }

                else if (tokenIs<CloseParenthesis>(current))
                {
                    depth--;
                }
            } while (depth > 0 && current < tokens.size() - 1);

            if (tokenIs<Equals>(current + 1))
            {
                if (tokenIs<OpenCurlyBracket>(current + 2))
                {
                    return parseDefine(pos);
                }

                tokenError(getToken(current + 2), "\"{\"");
            }
        }

        Token* expression = parseExpression(pos);

        if (AudioSource* audioSource = dynamic_cast<AudioSource*>(expression))
        {
            return new Play(audioSource->location, audioSource);
        }

        return expression;
    }

    Define* Parser::parseDefine(unsigned int pos)
    {
        const unsigned int start = pos;

        pos += 2;

        std::vector<InputDef*> inputs;

        if (!tokenIs<CloseParenthesis>(pos))
        {
            pos--;

            do
            {
                if (const Identifier* input = getToken<Identifier>(++pos))
                {
                    inputs.push_back(new InputDef(input->location, input->str));
                }

                else
                {
                    tokenError(getToken(pos), "input name");
                }

                pos++;
            } while (tokenIs<Comma>(pos));

            if (!tokenIs<CloseParenthesis>(pos))
            {
                tokenError(getToken(pos), "\")\"");
            }
        }

        FunctionDef* function = context->addFunction(getToken<Identifier>(start), inputs);

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

        pos += 2;

        const OpenCurlyBracket* open = getToken<OpenCurlyBracket>(pos++);

        std::vector<Token*> instructions;

        while (!tokenIs<CloseCurlyBracket>(pos))
        {
            Token* instruction = parseInstruction(pos);

            instructions.push_back(instruction);

            pos = instruction->location.end;
        }

        context = context->parent;

        return new Define(SourceLocation(path, function->location.line, function->location.character, function->location.start, pos + 1), instructions, function);
    }

    Assign* Parser::parseAssign(unsigned int pos) const
    {
        if (!tokenIs<Identifier>(pos))
        {
            tokenError(getToken(pos), "variable name");
        }

        VariableDef* variable = context->addVariable(getToken<Identifier>(pos));

        Token* value = parseExpression(pos + 2);

        return new Assign(SourceLocation(path, variable->location.line, variable->location.character, pos, value->location.end), variable, value);
    }

    Token* Parser::parseCall(unsigned int pos) const
    {
        const unsigned int start = pos;

        pos += 2;

        std::vector<Argument*> arguments;

        if (!tokenIs<CloseParenthesis>(pos))
        {
            pos--;

            do
            {
                Argument* argument = parseArgument(pos + 1);

                arguments.push_back(argument);

                pos = argument->location.end;
            } while (tokenIs<Comma>(pos));

            if (!tokenIs<CloseParenthesis>(pos))
            {
                tokenError(getToken(pos), "\")\"");
            }
        }

        const Identifier* name = getToken<Identifier>(start);

        const SourceLocation location(path, name->location.line, name->location.character, start, pos + 1);

        ArgumentList* argumentList = new ArgumentList(SourceLocation(path, name->location.line, name->location.character + 1, start + 1, pos), arguments, name->str);

        if (name->str == "time")
        {
            return new Time(location, argumentList);
        }

        if (name->str == "hold")
        {
            return new Hold(location, argumentList);
        }

        if (name->str == "lfo")
        {
            return new LFO(location, argumentList);
        }

        if (name->str == "sweep")
        {
            return new Sweep(location, argumentList);
        }

        if (name->str == "sequence")
        {
            return new Sequence(location, argumentList);
        }

        if (name->str == "repeat")
        {
            return new Repeat(location, argumentList);
        }

        if (name->str == "random")
        {
            return new Random(location, argumentList);
        }

        if (name->str == "limit")
        {
            return new Limit(location, argumentList);
        }

        if (name->str == "trigger")
        {
            return new Trigger(location, argumentList);
        }

        if (name->str == "if")
        {
            return new If(location, argumentList);
        }

        if (name->str == "all")
        {
            return new All(location, argumentList);
        }

        if (name->str == "any")
        {
            return new Any(location, argumentList);
        }

        if (name->str == "none")
        {
            return new None(location, argumentList);
        }

        if (name->str == "min")
        {
            return new Min(location, argumentList);
        }

        if (name->str == "max")
        {
            return new Max(location, argumentList);
        }

        if (name->str == "round")
        {
            return new Round(location, argumentList);
        }

        if (name->str == "sine")
        {
            return new Sine(location, argumentList);
        }

        if (name->str == "square")
        {
            return new Square(location, argumentList);
        }

        if (name->str == "triangle")
        {
            return new Triangle(location, argumentList);
        }

        if (name->str == "saw")
        {
            return new Saw(location, argumentList);
        }

        if (name->str == "noise")
        {
            return new Noise(location, argumentList);
        }

        if (name->str == "sample")
        {
            return new Sample(location, argumentList);
        }

        if (name->str == "oscillator")
        {
            return new Oscillator(location, argumentList);
        }

        if (name->str == "delay")
        {
            return new Delay(location, argumentList);
        }

        if (name->str == "include")
        {
            throw OrganicIncludeException("Includes must come before all other instructions.", location);
        }

        return new CallUser(location, argumentList, context->findFunction(name));
    }

    Argument* Parser::parseArgument(unsigned int pos) const
    {
        if (!tokenIs<Identifier>(pos))
        {
            tokenError(getToken(pos), "input name");
        }

        if (!tokenIs<Colon>(pos + 1))
        {
            tokenError(getToken(pos + 1), "\":\"");
        }

        const BasicToken* start = getToken(pos);

        Token* value = parseExpression(pos + 2);

        return new Argument(SourceLocation(path, start->location.line, start->location.character, pos, value->location.end), start->str, value);
    }

    Token* Parser::parseExpression(unsigned int pos) const
    {
        if (tokenIs<OpenSquareBracket>(pos))
        {
            return parseList(pos);
        }

        return parseTerms(pos);
    }

    List* Parser::parseList(unsigned int pos) const
    {
        const unsigned int start = pos;

        const BasicToken* startToken = getToken(start);

        if (tokenIs<CloseSquareBracket>(pos + 1))
        {
            throw OrganicParseException("Empty lists are not allowed.", SourceLocation(path, startToken->location.line, startToken->location.character, start, pos + 1));
        }

        std::vector<Token*> items;

        do
        {
            Token* token = parseExpression(pos + 1);

            items.push_back(token);

            pos = token->location.end;
        } while (tokenIs<Comma>(pos));

        if (!tokenIs<CloseSquareBracket>(pos))
        {
            tokenError(getToken(pos), "\"]\"");
        }

        return new List(SourceLocation(path, startToken->location.line, startToken->location.character, start, pos + 1), items);
    }

    Token* Parser::parseTerms(unsigned int pos) const
    {
        const unsigned int start = pos;

        std::vector<Token*> terms;

        while (true)
        {
            if (tokenIs<OpenParenthesis>(pos))
            {
                const unsigned int pStart = pos;

                Token* token = parseTerms(pos + 1);

                pos = token->location.end;

                if (!tokenIs<CloseParenthesis>(pos))
                {
                    tokenError(getToken(pos), "\")\"");
                }

                pos++;

                const BasicToken* startToken = getToken(pStart);

                terms.push_back(new ParenthesizedExpression(SourceLocation(path, startToken->location.line, startToken->location.character, pStart, pos), token));
            }

            else
            {
                Token* token = parseTerm(pos);

                terms.push_back(token);

                pos = token->location.end;
            }

            if (tokenIs<Operator>(pos))
            {
                terms.push_back(getToken(pos));

                pos++;
            }

            else
            {
                break;
            }
        }

        bool comparison = false;

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            const SourceLocation span(path, terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end);

            if (dynamic_cast<const DoubleEquals*>(terms[i]))
            {
                if (comparison)
                {
                    throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
                }

                comparison = true;

                terms[i - 1] = new EqualAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const Less*>(terms[i]))
            {
                if (comparison)
                {
                    throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
                }

                comparison = true;

                terms[i - 1] = new LessAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const Greater*>(terms[i]))
            {
                if (comparison)
                {
                    throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
                }

                comparison = true;

                terms[i - 1] = new GreaterAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const LessEqual*>(terms[i]))
            {
                if (comparison)
                {
                    throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
                }

                comparison = true;

                terms[i - 1] = new LessEqualAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const GreaterEqual*>(terms[i]))
            {
                if (comparison)
                {
                    throw OrganicParseException("Chaining comparison operators is not allowed.", terms[i]->location);
                }

                comparison = true;

                terms[i - 1] = new GreaterEqualAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }
        }

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            const SourceLocation span(path, terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end);

            if (dynamic_cast<const Power*>(terms[i]))
            {
                terms[i - 1] = new PowerAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }
        }

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            const SourceLocation span(path, terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end);

            if (dynamic_cast<const Multiply*>(terms[i]))
            {
                terms[i - 1] = new MultiplyAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const Divide*>(terms[i]))
            {
                terms[i - 1] = new DivideAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }
        }

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            const SourceLocation span(path, terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end);

            if (dynamic_cast<const Add*>(terms[i]))
            {
                terms[i - 1] = new AddAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const Subtract*>(terms[i]))
            {
                terms[i - 1] = new SubtractAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }
        }

        if (terms.size() != 1)
        {
            throw OrganicParseException("Invalid expression.", getToken(start)->location);
        }

        return terms[0];
    }

    Token* Parser::parseTerm(unsigned int pos) const
    {
        if (tokenIs<Value>(pos) || tokenIs<Constant>(pos) || tokenIs<String>(pos))
        {
            return getToken(pos);
        }

        if (tokenIs<Identifier>(pos))
        {
            if (tokenIs<OpenParenthesis>(pos + 1))
            {
                return parseCall(pos);
            }

            return context->findIdentifier(getToken<Identifier>(pos));
        }

        tokenError(getToken(pos), "expression term");

        return nullptr;
    }
}
