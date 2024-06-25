#include "../include/parse.h"

namespace Parser
{
    Parser::Parser(const std::string path) : path(path)
    {
        std::ifstream file(path);
        
        if (!file.is_open())
        {
            Utils::argumentError("Could not open \"" + path + "\".");
        }

        std::getline(file, code, std::string::traits_type::to_char_type(std::string::traits_type::eof()));

        file.close();
    }

    Program* Parser::parse()
    {
        tokenize();

        current = 0;

        std::vector<const Instruction*> instructions;

        while (current < tokens.size())
        {
            const Instruction* token = (const Instruction*)parseInstruction(current);

            instructions.push_back(token);

            current = token->location.end;
        }

        return new Program(instructions);
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
                tokens.push_back(new OpenParenthesis(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ')')
            {
                tokens.push_back(new CloseParenthesis(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '{')
            {
                tokens.push_back(new OpenCurlyBracket(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '}')
            {
                tokens.push_back(new CloseCurlyBracket(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ':')
            {
                tokens.push_back(new Colon(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ',')
            {
                tokens.push_back(new Comma(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '=')
            {
                nextCharacter();

                if (code[current] == '=')
                {
                    tokens.push_back(new DoubleEquals(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                    nextCharacter();
                }

                else
                {
                    tokens.push_back(new Equals(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));
                }
            }

            else if (code[current] == '+')
            {
                tokens.push_back(new Add(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '-')
            {
                tokens.push_back(new Subtract(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '*')
            {
                tokens.push_back(new Multiply(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '/')
            {
                tokens.push_back(new Divide(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '^')
            {
                tokens.push_back(new Power(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '<')
            {
                nextCharacter();

                if (code[current] == '=')
                {
                    tokens.push_back(new LessEqual(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                    nextCharacter();
                }

                else
                {
                    tokens.push_back(new Less(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));
                }
            }

            else if (code[current] == '>')
            {
                nextCharacter();

                if (code[current] == '=')
                {
                    tokens.push_back(new GreaterEqual(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));

                    nextCharacter();
                }

                else
                {
                    tokens.push_back(new Greater(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1)));
                }
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

                if (dynamic_cast<Subtract*>(tokens.back()) && (dynamic_cast<Equals*>(tokens[tokens.size() - 2]) || dynamic_cast<Colon*>(tokens[tokens.size() - 2]) || dynamic_cast<OpenParenthesis*>(tokens[tokens.size() - 2]) || dynamic_cast<Operator*>(tokens[tokens.size() - 2])))
                {
                    tokens[tokens.size() - 1] = new Value(ParserLocation(tokens.back()->location.line, tokens.back()->location.character, tokens.size() - 1, tokens.size()), "-" + constant, std::stod("-" + constant));
                }

                else
                {
                    tokens.push_back(new Value(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1), constant, std::stod(constant)));
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
                            tokens.push_back(new Value(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1), name, getFrequency(base + 12 * (name[1] - 48))));
                        }

                        else if (name.size() == 3 && isdigit(name[2]))
                        {
                            if (name[1] == 's')
                            {
                                tokens.push_back(new Value(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1), name, getFrequency(base + 12 * (name[2] - 48) + 1)));
                            }

                            else if (name[1] == 'f')
                            {
                                tokens.push_back(new Value(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1), name, getFrequency(base + 12 * (name[2] - 48) - 1)));
                            }

                            else
                            {
                                tokens.push_back(new String(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1), name));
                            }
                        }

                        else
                        {
                            tokens.push_back(new String(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1), name));
                        }
                    }

                    else
                    {
                        tokens.push_back(new String(ParserLocation(startLine, startCharacter, tokens.size(), tokens.size() + 1), name));
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

    const Token* Parser::getToken(const unsigned int pos) const
    {
        if (pos >= tokens.size())
        {
            Utils::parseError("Unexpected end of file.", path, line, character);

            return nullptr;
        }

        return tokens[pos];
    }

    template <typename T> const T* Parser::getToken(const unsigned int pos) const
    {
        return dynamic_cast<const T*>(getToken(pos));
    }

    template <typename T> bool Parser::tokenIs(const unsigned int pos) const
    {
        if (pos >= tokens.size())
        {
            return false;
        }

        return dynamic_cast<const T*>(getToken(pos));
    }

    void Parser::tokenError(const Token* token, const std::string expected) const
    {
        Utils::parseError("Expected " + expected + ", received \"" + token->str + "\".", path, token->location.line, token->location.character);
    }

    const Token* Parser::parseInstruction(unsigned int pos) const
    {
        if (!tokenIs<String>(pos))
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
            } while (depth > 0);

            if (tokenIs<Equals>(pos + 1))
            {
                if (tokenIs<OpenCurlyBracket>(pos + 2))
                {
                    return parseDefine(start);
                }

                tokenError(getToken(pos + 2), "\"{\"");
            }

            return parseCall(start);
        }

        tokenError(getToken(pos + 1), "\"=\" or \"(\"");

        return nullptr;
    }

    const Token* Parser::parseDefine(unsigned int pos) const
    {
        const Token* name = getToken(pos);
        
        pos += 2;

        std::vector<std::string> inputs;

        if (!tokenIs<CloseParenthesis>(pos))
        {
            pos--;

            do
            {
                const String* input = getToken<String>(++pos);

                if (!input)
                {
                    tokenError(getToken(pos), "input name");
                }

                inputs.push_back(input->str);

                pos++;
            } while (tokenIs<Comma>(pos));

            if (!tokenIs<CloseParenthesis>(pos))
            {
                tokenError(getToken(pos), "\")\"");
            }
        }

        pos += 2;

        const CodeBlock* body = dynamic_cast<const CodeBlock*>(parseCodeBlock(pos));

        return new Define(ParserLocation(name->location.line, name->location.character, name->location.start, body->location.end), name->str, inputs, body);
    }

    const Token* Parser::parseAssign(unsigned int pos) const
    {
        const Token* name = getToken(pos);
        const Token* value = parseExpression(pos + 2);

        return new Assign(ParserLocation(name->location.line, name->location.character, pos, value->location.end), name->str, value);
    }

    const Token* Parser::parseCodeBlock(unsigned int pos) const
    {
        const unsigned int start = pos;

        const OpenCurlyBracket* open = getToken<OpenCurlyBracket>(pos++);

        std::vector<const Instruction*> instructions;

        while (!tokenIs<CloseCurlyBracket>(pos))
        {
            const Instruction* instruction = (const Instruction*)parseInstruction(pos);

            instructions.push_back(instruction);

            pos = instruction->location.end;
        }

        return new CodeBlock(ParserLocation(open->location.line, open->location.character, start, pos + 1), instructions);
    }

    const Token* Parser::parseCall(unsigned int pos) const
    {
        const unsigned int start = pos;

        pos += 2;

        std::vector<const Argument*> arguments;

        if (!tokenIs<CloseParenthesis>(pos))
        {
            pos--;

            do
            {
                const Argument* argument = (const Argument*)parseArgument(pos + 1);

                arguments.push_back(argument);

                pos = argument->location.end;
            } while (tokenIs<Comma>(pos));

            if (!tokenIs<CloseParenthesis>(pos))
            {
                tokenError(getToken(pos), "\")\"");
            }
        }

        const Token* str = getToken(start);

        return new Call(ParserLocation(str->location.line, str->location.character, start, pos + 1), str->str, new ArgumentList(arguments, str->str, path));
    }

    const Token* Parser::parseArgument(unsigned int pos) const
    {
        if (!tokenIs<String>(pos))
        {
            tokenError(getToken(pos), "input name");
        }

        if (!tokenIs<Colon>(pos + 1))
        {
            tokenError(getToken(pos + 1), "\":\"");
        }

        const Token* value = parseExpression(pos + 2);

        return new Argument(ParserLocation(value->location.line, value->location.character, pos, value->location.end), getToken(pos)->str, value);
    }

    const Token* Parser::parseExpression(unsigned int pos) const
    {
        if (tokenIs<OpenParenthesis>(pos) && tokenIs<Comma>(parseExpression(pos + 1)->location.end))
        {
            const unsigned int start = pos;

            std::vector<const Token*> list;

            do
            {
                const Token* expression = parseExpression(pos + 1);

                list.push_back(expression);

                pos = expression->location.end;
            } while (tokenIs<Comma>(pos));

            if (!tokenIs<CloseParenthesis>(pos))
            {
                tokenError(getToken(pos), "\")\"");
            }

            const Token* token = getToken(start);

            if (list.size() == 0)
            {
                tokenError(token, "expression");
            }

            return new List(ParserLocation(token->location.line, token->location.character , start, pos + 1), list);
        }

        return parseTerms(pos);
    }

    const Token* Parser::parseTerms(unsigned int pos) const
    {
        const unsigned int start = pos;

        std::vector<const Token*> terms;

        while (true)
        {
            if (tokenIs<OpenParenthesis>(pos))
            {
                const unsigned int pStart = pos;

                const Token* token = parseTerms(pos + 1);

                pos = token->location.end;

                if (!tokenIs<CloseParenthesis>(pos))
                {
                    tokenError(getToken(pos), "\")\"");

                    return nullptr;
                }

                pos++;

                const Token* startToken = getToken(pStart);

                terms.push_back(new ParenthesizedExpression(ParserLocation(startToken->location.line, startToken->location.character, pStart, pos), token));
            }

            else
            {
                const Token* token = parseTerm(pos);

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
            if (dynamic_cast<const DoubleEquals*>(terms[i]))
            {
                if (comparison)
                {
                    Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->location.line, terms[i]->location.character);
                }

                comparison = true;

                terms[i - 1] = new EqualsObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const Less*>(terms[i]))
            {
                if (comparison)
                {
                    Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->location.line, terms[i]->location.character);
                }

                comparison = true;

                terms[i - 1] = new LessObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const Greater*>(terms[i]))
            {
                if (comparison)
                {
                    Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->location.line, terms[i]->location.character);
                }

                comparison = true;
                
                terms[i - 1] = new GreaterObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const LessEqual*>(terms[i]))
            {
                if (comparison)
                {
                    Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->location.line, terms[i]->location.character);
                }

                comparison = true;

                terms[i - 1] = new LessEqualObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const GreaterEqual*>(terms[i]))
            {
                if (comparison)
                {
                    Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->location.line, terms[i]->location.character);
                }

                comparison = true;
                
                terms[i - 1] = new GreaterEqualObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + 1, terms.begin() + i + 2);

                i--;
            }
        }

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            if (dynamic_cast<const Power*>(terms[i]))
            {
                terms[i - 1] = new PowerObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }
        }

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            if (dynamic_cast<const Multiply*>(terms[i]))
            {
                terms[i - 1] = new MultiplyObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const Divide*>(terms[i]))
            {
                terms[i - 1] = new DivideObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }
        }

        for (unsigned int i = 1; i < terms.size() - 1; i++)
        {
            if (dynamic_cast<const Add*>(terms[i]))
            {
                terms[i - 1] = new AddObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }

            else if (dynamic_cast<const Subtract*>(terms[i]))
            {
                terms[i - 1] = new SubtractObject(ParserLocation(terms[i - 1]->location.line, terms[i - 1]->location.character, terms[i - 1]->location.start, terms[i + 1]->location.end), terms[i - 1], terms[i + 1]);

                terms.erase(terms.begin() + i, terms.begin() + i + 2);

                i--;
            }
        }

        if (terms.size() != 1)
        {
            const Token* startToken = getToken(start);

            Utils::parseError("Invalid expression.", path, startToken->location.line, startToken->location.character);
        }

        return foldConstants(terms[0]);
    }

    const Token* Parser::parseTerm(unsigned int pos) const
    {
        if (tokenIs<Value>(pos))
        {
            return getToken(pos);
        }

        if (tokenIs<String>(pos))
        {
            if (tokenIs<OpenParenthesis>(pos + 1))
            {
                return parseCall(pos);
            }

            const Token* str = getToken(pos);

            if (str->str == "sequence-forwards" ||
                str->str == "sequence-backwards" ||
                str->str == "sequence-ping-pong" ||
                str->str == "sequence-random" ||
                str->str == "random-step" ||
                str->str == "random-linear" ||
                str->str == "pi" ||
                str->str == "e")
            {
                return new NamedConstant(ParserLocation(str->location.line, str->location.character, pos, pos + 1), str->str);
            }

            return new Variable(ParserLocation(str->location.line, str->location.character, pos, pos + 1), str->str);
        }

        tokenError(getToken(pos), "expression term");

        return nullptr;
    }

    const Token* Parser::foldConstants(const Token* token) const
    {
        if (const ParenthesizedExpression* object = dynamic_cast<const ParenthesizedExpression*>(token))
        {
            const Token* result = foldConstants(object->value);

            if (const Value* value = dynamic_cast<const Value*>(result))
            {
                return new Value(object->location, object->str, value->value);
            }

            return new ParenthesizedExpression(object->location, result);
        }

        if (const PowerObject* object = dynamic_cast<const PowerObject*>(token))
        {
            const Token* left = foldConstants(object->value1);
            const Token* right = foldConstants(object->value2);

            const Value* left_value = dynamic_cast<const Value*>(left);
            const Value* right_value = dynamic_cast<const Value*>(right);

            if (left_value && right_value)
            {
                return new Value(ParserLocation(left_value->location.line, left_value->location.character, left_value->location.start, right_value->location.end), object->str, pow(left_value->value, right_value->value));
            }

            return new PowerObject(ParserLocation(left->location.line, left->location.character, left->location.start, right->location.end), left, right);
        }
        
        if (const MultiplyObject* object = dynamic_cast<const MultiplyObject*>(token))
        {
            const Token* left = foldConstants(object->value1);
            const Token* right = foldConstants(object->value2);

            const Value* left_value = dynamic_cast<const Value*>(left);
            const Value* right_value = dynamic_cast<const Value*>(right);

            if (left_value && right_value)
            {
                return new Value(ParserLocation(left_value->location.line, left_value->location.character, left_value->location.start, right_value->location.end), object->str, left_value->value * right_value->value);
            }

            return new MultiplyObject(ParserLocation(left->location.line, left->location.character, left->location.start, right->location.end), left, right);
        }

        if (const DivideObject* object = dynamic_cast<const DivideObject*>(token))
        {
            const Token* left = foldConstants(object->value1);
            const Token* right = foldConstants(object->value2);

            const Value* left_value = dynamic_cast<const Value*>(left);
            const Value* right_value = dynamic_cast<const Value*>(right);

            if (left_value && right_value)
            {
                return new Value(ParserLocation(left_value->location.line, left_value->location.character, left_value->location.start, right_value->location.end), object->str, left_value->value / right_value->value);
            }

            return new DivideObject(ParserLocation(left->location.line, left->location.character, left->location.start, right->location.end), left, right);
        }

        if (const AddObject* object = dynamic_cast<const AddObject*>(token))
        {
            const Token* left = foldConstants(object->value1);
            const Token* right = foldConstants(object->value2);

            const Value* left_value = dynamic_cast<const Value*>(left);
            const Value* right_value = dynamic_cast<const Value*>(right);

            if (left_value && right_value)
            {
                return new Value(ParserLocation(left_value->location.line, left_value->location.character, left_value->location.start, right_value->location.end), object->str, left_value->value + right_value->value);
            }

            return new AddObject(ParserLocation(left->location.line, left->location.character, left->location.start, right->location.end), left, right);
        }

        if (const SubtractObject* object = dynamic_cast<const SubtractObject*>(token))
        {
            const Token* left = foldConstants(object->value1);
            const Token* right = foldConstants(object->value2);

            const Value* left_value = dynamic_cast<const Value*>(left);
            const Value* right_value = dynamic_cast<const Value*>(right);

            if (left_value && right_value)
            {
                return new Value(ParserLocation(left_value->location.line, left_value->location.character, left_value->location.start, right_value->location.end), object->str, left_value->value - right_value->value);
            }

            return new SubtractObject(ParserLocation(left->location.line, left->location.character, left->location.start, right->location.end), left, right);
        }

        return token;
    }

    double Parser::getFrequency(const double note) const
    {
        return 440 * pow(2, (note - 57) / 12);
    }
}
