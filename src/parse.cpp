#include "../include/parse.h"

namespace Parser
{
    Parser::Parser(const Path* path, std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths) :
        path(path), includedPaths(includedPaths)
    {
        std::ifstream file(path->string());

        if (!file.is_open())
        {
            Utils::fileError("Could not open \"" + path->string() + "\".");
        }

        std::getline(file, code, std::string::traits_type::to_char_type(std::string::traits_type::eof()));

        file.close();
    }

    Program* Parser::parse()
    {
        tokenize();

        current = 0;

        std::vector<Token*> instructions;

        while (current < tokens.size())
        {
            Token* token = parseInstruction(current);

            instructions.push_back(token);

            current = token->location.end;
        }

        if (instructions.empty())
        {
            return new Program(SourceLocation(path, 0, 0, 0, 0), instructions);
        }

        return new Program(SourceLocation(path, instructions[0]->location.line, instructions[0]->location.character, instructions[0]->location.start, instructions.back()->location.end), instructions);
    }

    void Parser::skipWhitespace()
    {
        while (current < code.size() && isspace(code[current]))
        {
            nextCharacter();
        }
    }

    void Parser::nextCharacter()
    {
        if (code[current] == '\n')
        {
            line++;
            character = 0;
        }

        current++;
        character++;
    }

    void Parser::tokenize()
    {
        while (current < code.size())
        {
            skipWhitespace();

            unsigned int startLine = line;
            unsigned int startCharacter = character;

            if (code[current] == '/' && current < code.size() - 1 && code[current + 1] == '/')
            {
                while (current < code.size() && code[current] != '\n')
                {
                    nextCharacter();
                }
            }

            else if (code[current] == '/' && current < code.size() - 1 && code[current + 1] == '*')
            {
                nextCharacter();
                nextCharacter();

                while (current < code.size() - 1 && !(code[current] == '*' && code[current + 1] == '/'))
                {
                    nextCharacter();
                }

                nextCharacter();
                nextCharacter();
            }

            else if (code[current] == '(')
            {
                tokens.push_back(new OpenParenthesis(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ')')
            {
                tokens.push_back(new CloseParenthesis(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '[')
            {
                tokens.push_back(new OpenSquareBracket(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ']')
            {
                tokens.push_back(new CloseSquareBracket(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '{')
            {
                tokens.push_back(new OpenCurlyBracket(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '}')
            {
                tokens.push_back(new CloseCurlyBracket(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ':')
            {
                tokens.push_back(new Colon(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ',')
            {
                tokens.push_back(new Comma(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '=')
            {
                nextCharacter();

                if (code[current] == '=')
                {
                    tokens.push_back(new DoubleEquals(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                    nextCharacter();
                }

                else
                {
                    tokens.push_back(new Equals(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));
                }
            }

            else if (code[current] == '+')
            {
                tokens.push_back(new Add(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '-')
            {
                tokens.push_back(new Subtract(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '*')
            {
                tokens.push_back(new Multiply(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '/')
            {
                tokens.push_back(new Divide(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '^')
            {
                tokens.push_back(new Power(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '<')
            {
                nextCharacter();

                if (code[current] == '=')
                {
                    tokens.push_back(new LessEqual(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                    nextCharacter();
                }

                else
                {
                    tokens.push_back(new Less(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));
                }
            }

            else if (code[current] == '>')
            {
                nextCharacter();

                if (code[current] == '=')
                {
                    tokens.push_back(new GreaterEqual(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                    nextCharacter();
                }

                else
                {
                    tokens.push_back(new Greater(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1)));
                }
            }

            else if (code[current] == '"')
            {
                nextCharacter();

                std::string str;

                while (current < code.size() && code[current] != '"')
                {
                    str += code[current];

                    nextCharacter();
                }

                if (code[current] != '"')
                {
                    Utils::parseError("Unexpected end of file.", path, line, character);
                }

                tokens.push_back(new String(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1), str));

                nextCharacter();
            }

            else if (isdigit(code[current]))
            {
                std::string constant;

                bool period = false;

                while (current < code.size() && (isdigit(code[current]) || code[current] == '.'))
                {
                    if (code[current] == '.')
                    {
                        if (period)
                        {
                            Utils::parseError("Constants cannot contain more than one decimal point.", path, startLine, startCharacter);
                        }

                        period = true;
                    }

                    constant += code[current];

                    nextCharacter();
                }

                if (!tokens.empty() && tokenIs<Subtract>(tokens.size() - 1) && (tokens.size() < 2 || (!tokenIs<Identifier>(tokens.size() - 2) && !tokenIs<Value>(tokens.size() - 2))))
                {
                    tokens[tokens.size() - 1] = new Value(SourceLocation(path, tokens.back()->location.line, tokens.back()->location.character, tokens.size() - 1, tokens.size()), "-" + constant, std::stod("-" + constant));
                }

                else
                {
                    tokens.push_back(new Value(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1), constant, std::stod(constant)));
                }
            }

            else
            {
                if (isalpha(code[current]) || code[current] == '_')
                {
                    std::string name;

                    while (current < code.size() && (isalnum(code[current]) || code[current] == '-' || code[current] == '_'))
                    {
                        name += code[current];

                        nextCharacter();
                    }

                    double base = 0;

                    bool match = true;

                    switch (name[0])
                    {
                        case 'c':
                            break;

                        case 'd':
                            base = 2;

                            break;

                        case 'e':
                            base = 4;

                            break;

                        case 'f':
                            base = 5;

                            break;

                        case 'g':
                            base = 7;

                            break;

                        case 'a':
                            base = 9;

                            break;

                        case 'b':
                            base = 11;

                            break;

                        default:
                            match = false;

                            break;
                    }

                    if (match)
                    {
                        if (name.size() == 2 && isdigit(name[1]))
                        {
                            tokens.push_back(new Value(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1), name, getFrequency(base + 12 * (name[1] - 48))));
                        }

                        else if (name.size() == 3 && isdigit(name[2]))
                        {
                            if (name[1] == 's')
                            {
                                tokens.push_back(new Value(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1), name, getFrequency(base + 12 * (name[2] - 48) + 1)));
                            }

                            else if (name[1] == 'f')
                            {
                                tokens.push_back(new Value(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1), name, getFrequency(base + 12 * (name[2] - 48) - 1)));
                            }

                            else
                            {
                                tokens.push_back(new Identifier(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1), name));
                            }
                        }

                        else
                        {
                            tokens.push_back(new Identifier(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1), name));
                        }
                    }

                    else
                    {
                        tokens.push_back(new Identifier(SourceLocation(path, startLine, startCharacter, tokens.size(), tokens.size() + 1), name));
                    }
                }

                else
                {
                    Utils::parseError("Unrecognized symbol \"" + std::string(1, code[current]) + "\".", path, line, character);
                }
            }

            skipWhitespace();
        }
    }

    BasicToken* Parser::getToken(const unsigned int pos) const
    {
        if (pos >= tokens.size())
        {
            Utils::parseError("Unexpected end of file.", path, line, character);

            return nullptr;
        }

        return tokens[pos];
    }

    template <typename T> T* Parser::getToken(const unsigned int pos) const
    {
        return dynamic_cast<T*>(getToken(pos));
    }

    template <typename T> bool Parser::tokenIs(const unsigned int pos) const
    {
        if (pos >= tokens.size())
        {
            return false;
        }

        return dynamic_cast<T*>(getToken(pos));
    }

    void Parser::tokenError(const BasicToken* token, const std::string expected) const
    {
        Utils::parseError("Expected " + expected + ", received \"" + token->str + "\".", token->location);
    }

    Token* Parser::parseInstruction(unsigned int pos) const
    {
        if (!tokenIs<Identifier>(pos))
        {
            tokenError(getToken(pos), "variable or function name");
        }

        if (tokenIs<Equals>(pos + 1))
        {
            return parseAssign(pos);
        }

        if (tokenIs<OpenParenthesis>(pos + 1))
        {
            unsigned int start = pos;

            unsigned int depth = 0;

            do
            {
                pos++;

                if (tokenIs<OpenParenthesis>(pos))
                {
                    depth++;
                }

                else if (tokenIs<CloseParenthesis>(pos))
                {
                    depth--;
                }
            } while (depth > 0 && pos < tokens.size());

            if (tokenIs<Equals>(pos + 1))
            {
                if (tokenIs<OpenCurlyBracket>(pos + 2))
                {
                    return parseDefine(start);
                }

                tokenError(getToken(pos + 2), "\"{\"");
            }

            return parseCall(start, true);
        }

        tokenError(getToken(pos + 1), "\"=\" or \"(\"");

        return nullptr;
    }

    Define* Parser::parseDefine(unsigned int pos) const
    {
        const BasicToken* name = getToken(pos);

        pos += 2;

        std::vector<Identifier*> inputs;

        if (!tokenIs<CloseParenthesis>(pos))
        {
            pos--;

            do
            {
                if (Identifier* input = getToken<Identifier>(++pos))
                {
                    inputs.push_back(input);
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

        pos += 2;

        const OpenCurlyBracket* open = getToken<OpenCurlyBracket>(pos++);

        std::vector<Token*> instructions;

        while (!tokenIs<CloseCurlyBracket>(pos))
        {
            Token* instruction = parseInstruction(pos);

            instructions.push_back(instruction);

            pos = instruction->location.end;
        }

        return new Define(SourceLocation(path, name->location.line, name->location.character, name->location.start, pos + 1), name->str, inputs, instructions);
    }

    Assign* Parser::parseAssign(unsigned int pos) const
    {
        const BasicToken* name = getToken(pos);

        Token* value = parseExpression(pos + 2);

        return new Assign(SourceLocation(path, name->location.line, name->location.character, pos, value->location.end), name->str, value);
    }

    Token* Parser::parseCall(unsigned int pos, const bool topLevel) const
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

        const BasicToken* str = getToken(start);

        if (str->str == "include")
        {
            if (!topLevel)
            {
                Utils::parseError("Cannot call function \"include\" here.", str->location);
            }

            if (arguments.empty())
            {
                Utils::includeWarning("This include does not specify a source file, it will have no effect.", str->location);

                return new Ignore(SourceLocation(path, str->location.line, str->location.character, start, pos + 1));
            }

            if (arguments[0]->name != "file")
            {
                Utils::parseError("Invalid input name \"" + arguments[0]->name + "\" for function \"include\".", arguments[0]->location);
            }

            if (arguments.size() > 1)
            {
                Utils::parseError("Invalid input name \"" + arguments[1]->name + "\" for function \"include\".", arguments[1]->location);
            }

            if (const String* file = dynamic_cast<const String*>(arguments[0]->value))
            {
                const Path* sourcePath = Path::beside(file->str, path);

                if (!sourcePath->exists())
                {
                    Utils::includeError("Source file \"" + sourcePath->string() + "\" does not exist.", str->location);
                }

                if (!sourcePath->isFile())
                {
                    Utils::includeError("\"" + sourcePath->string() + "\" is not a file.", str->location);
                }

                if (includedPaths.count(sourcePath))
                {
                    Utils::includeWarning("Source file \"" + sourcePath->string() + "\" has already been included, this include will be ignored.", file->location);

                    return new Ignore(SourceLocation(path, str->location.line, str->location.character, start, pos + 1));
                }

                includedPaths.insert(sourcePath);

                return new Include(SourceLocation(path, str->location.line, str->location.character, start, pos + 1), (new Parser(sourcePath, includedPaths))->parse());
            }

            Utils::parseError("Expected \"string\".", arguments[0]->value->location);
        }

        const SourceLocation location(path, str->location.line, str->location.character, start, pos + 1);

        const ArgumentList* argumentList = new ArgumentList(arguments, str->str);

        if (str->str == "time")
        {
            return new Time(location, argumentList, topLevel);
        }

        if (str->str == "hold")
        {
            return new Hold(location, argumentList, topLevel);
        }

        if (str->str == "lfo")
        {
            return new LFO(location, argumentList, topLevel);
        }

        if (str->str == "sweep")
        {
            return new Sweep(location, argumentList, topLevel);
        }

        if (str->str == "sequence")
        {
            return new Sequence(location, argumentList, topLevel);
        }

        if (str->str == "repeat")
        {
            return new Repeat(location, argumentList, topLevel);
        }

        if (str->str == "random")
        {
            return new Random(location, argumentList, topLevel);
        }

        if (str->str == "limit")
        {
            return new Limit(location, argumentList, topLevel);
        }

        if (str->str == "trigger")
        {
            return new Trigger(location, argumentList, topLevel);
        }

        if (str->str == "if")
        {
            return new If(location, argumentList, topLevel);
        }

        if (str->str == "sine")
        {
            return new Sine(location, argumentList, topLevel);
        }

        if (str->str == "square")
        {
            return new Square(location, argumentList, topLevel);
        }

        if (str->str == "triangle")
        {
            return new Triangle(location, argumentList, topLevel);
        }

        if (str->str == "saw")
        {
            return new Saw(location, argumentList, topLevel);
        }

        if (str->str == "noise")
        {
            return new Noise(location, argumentList, topLevel);
        }

        if (str->str == "sample")
        {
            return new Sample(location, argumentList, topLevel);
        }

        if (str->str == "delay")
        {
            return new Delay(location, argumentList, topLevel);
        }

        if (str->str == "perform")
        {
            return new Perform(location, argumentList, topLevel);
        }

        return new CallUser(location, str->str, argumentList, topLevel);
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
            Utils::parseError("Empty lists are not allowed.", SourceLocation(path, startToken->location.line, startToken->location.character, start, pos + 1));
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
            const SourceLocation span = SourceLocation(path, terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end);

            if (dynamic_cast<const DoubleEquals*>(terms[i]))
            {
                if (comparison)
                {
                    Utils::parseError("Chaining comparison operators is not allowed.", terms[i]->location);
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
                    Utils::parseError("Chaining comparison operators is not allowed.", terms[i]->location);
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
                    Utils::parseError("Chaining comparison operators is not allowed.", terms[i]->location);
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
                    Utils::parseError("Chaining comparison operators is not allowed.", terms[i]->location);
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
                    Utils::parseError("Chaining comparison operators is not allowed.", terms[i]->location);
                }

                comparison = true;

                terms[i - 1] = new GreaterEqualAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }
        }

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            const SourceLocation span = SourceLocation(path, terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end);

            if (dynamic_cast<const Power*>(terms[i]))
            {
                terms[i - 1] = new PowerAlias(span, terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }
        }

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            const SourceLocation span = SourceLocation(path, terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end);

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
            const SourceLocation span = SourceLocation(path, terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end);

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
            Utils::parseError("Invalid expression.", getToken(start)->location);
        }

        return terms[0];
    }

    Token* Parser::parseTerm(unsigned int pos) const
    {
        if (tokenIs<Value>(pos) || tokenIs<String>(pos))
        {
            return getToken(pos);
        }

        if (tokenIs<Identifier>(pos))
        {
            if (tokenIs<OpenParenthesis>(pos + 1))
            {
                return parseCall(pos, false);
            }

            Identifier* token = getToken<Identifier>(pos);

            if (token->str == "sequence-forwards" ||
                token->str == "sequence-backwards" ||
                token->str == "sequence-ping-pong" ||
                token->str == "sequence-random" ||
                token->str == "random-step" ||
                token->str == "random-linear" ||
                token->str == "pi" ||
                token->str == "e")
            {
                return new NamedConstant(SourceLocation(path, token->location.line, token->location.character, pos, pos + 1), token->str);
            }

            return token;
        }

        tokenError(getToken(pos), "expression term");

        return nullptr;
    }

    double Parser::getFrequency(const double note) const
    {
        return 440 * pow(2, (note - 57) / 12);
    }
}
