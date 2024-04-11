#include "../include/parse.h"

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

    std::vector<Instruction*> instructions;

    while (current < tokens.size())
    {
        const TokenRange* range = parseInstruction(current);

        instructions.push_back((Instruction*)range->token);

        current = range->end;
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
            tokens.push_back(new OpenParenthesis(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == ')')
        {
            tokens.push_back(new CloseParenthesis(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == '{')
        {
            tokens.push_back(new OpenCurlyBracket(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == '}')
        {
            tokens.push_back(new CloseCurlyBracket(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == ':')
        {
            tokens.push_back(new Colon(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == ',')
        {
            tokens.push_back(new Comma(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == '=')
        {
            nextCharacter();

            if (code[current] == '=')
            {
                tokens.push_back(new DoubleEqualToken(startLine, startCharacter));

                nextCharacter();
            }

            else
            {
                tokens.push_back(new EqualToken(startLine, startCharacter));
            }
        }

        else if (code[current] == '+')
        {
            tokens.push_back(new AddToken(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == '-')
        {
            tokens.push_back(new SubtractToken(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == '*')
        {
            tokens.push_back(new MultiplyToken(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == '/')
        {
            tokens.push_back(new DivideToken(startLine, startCharacter));

            nextCharacter();
        }

        else if (code[current] == '<')
        {
            nextCharacter();

            if (code[current] == '=')
            {
                tokens.push_back(new LessEqualToken(startLine, startCharacter));

                nextCharacter();
            }

            else
            {
                tokens.push_back(new LessToken(startLine, startCharacter));
            }
        }

        else if (code[current] == '>')
        {
            nextCharacter();

            if (code[current] == '=')
            {
                tokens.push_back(new GreaterEqualToken(startLine, startCharacter));

                nextCharacter();
            }

            else
            {
                tokens.push_back(new GreaterToken(startLine, startCharacter));
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

            tokens.push_back(new Constant(startLine, startCharacter, constant));
        }

        else
        {
            bool value = false;

            if (code[current] == '#')
            {
                value = true;

                nextCharacter();
            }

            if (isalpha(code[current]) || code[current] == '_')
            {
                std::string name;

                while (current < code.size() && (isalnum(code[current]) || code[current] == '-' || code[current] == '_'))
                {
                    name += code[current];

                    nextCharacter();
                }

                tokens.push_back(new Name(startLine, startCharacter, name, value));
            }

            else
            {
                if (value)
                {
                    Utils::parseError("Expected variable name, received \"" + std::string(1, code[current]) + "\".", path, line, character);
                }

                else
                {
                    Utils::parseError("Unrecognized symbol \"" + std::string(1, code[current]) + "\".", path, line, character);
                }
            }
        }

        skipWhitespace();
    }
}

Token* Parser::getToken(const unsigned int pos) const
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

void Parser::tokenError(const Token* token, const std::string expected) const
{
    Utils::parseError("Expected " + expected + ", received \"" + token->string() + "\".", path, token->line, token->character);
}

TokenRange* Parser::parseInstruction(unsigned int pos) const
{
    if (!tokenIs<Name>(pos))
    {
        tokenError(getToken(pos), "name");
    }

    if (tokenIs<EqualToken>(pos + 1))
    {
        return parseAssign(pos);
    }

    if (tokenIs<OpenParenthesis>(pos + 1))
    {
        return parseCall(pos);
    }

    tokenError(getToken(pos + 1), "\"=\" or \"(\"");

    return nullptr;
}

TokenRange* Parser::parseAssign(unsigned int pos) const
{
    const TokenRange* range = parseExpression(pos + 2);

    return new TokenRange(pos, range->end, new Assign(getToken<Name>(pos), range->token));
}

TokenRange* Parser::parseCodeBlock(unsigned int pos) const
{
    const unsigned int start = pos;

    const OpenCurlyBracket* open = getToken<OpenCurlyBracket>(pos++);

    std::vector<Instruction*> instructions;

    while (!tokenIs<CloseCurlyBracket>(pos))
    {
        const TokenRange* range = parseInstruction(pos);

        instructions.push_back((Instruction*)range->token);

        pos = range->end;
    }

    return new TokenRange(start, pos + 1, new CodeBlock(open->line, open->character, instructions));
}

TokenRange* Parser::parseCall(unsigned int pos) const
{
    const unsigned int start = pos;

    pos += 2;

    std::vector<Argument*> arguments;

    if (tokenIs<Name>(pos))
    {
        pos -= 1;

        do
        {
            const TokenRange* range = parseArgument(pos + 1);

            arguments.push_back((Argument*)range->token);

            pos = range->end;
        } while (tokenIs<Comma>(pos));
    }

    if (tokenIs<CloseParenthesis>(pos))
    {
        const Name* name = getToken<Name>(start);

        return new TokenRange(start, pos + 1, new Call(name, new ArgumentList(arguments, name->name, path)));
    }

    tokenError(getToken(pos), "\")\"");

    return nullptr;
}

TokenRange* Parser::parseArgument(unsigned int pos) const
{
    if (!tokenIs<Name>(pos))
    {
        tokenError(getToken(pos), "input name");
    }

    if (!tokenIs<Colon>(pos + 1))
    {
        tokenError(getToken(pos + 1), "\":\"");
    }

    const TokenRange* range = parseExpression(pos + 2);

    const Name* name = getToken<Name>(pos);

    return new TokenRange(pos, range->end, new Argument(name->line, name->character, name->name, range->token));
}

TokenRange* Parser::parseExpression(unsigned int pos) const
{
    if (tokenIs<OpenCurlyBracket>(pos))
    {
        return parseCodeBlock(pos);
    }

    if (tokenIs<OpenParenthesis>(pos) && tokenIs<Comma>(parseExpression(pos + 1)->end))
    {
        const unsigned int start = pos;

        std::vector<Token*> list;

        do
        {
            const TokenRange* range = parseExpression(pos + 1);

            list.push_back(range->token);

            pos = range->end;
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

        return new TokenRange(start, pos + 1, new ListToken(token->line, token->character, list));
    }

    return parseTerms(pos);
}

TokenRange* Parser::parseTerms(unsigned int pos) const
{
    const unsigned int start = pos;

    std::vector<TokenRange*> terms;

    do
    {
        if (tokenIs<Operator>(pos) && terms.size() > 0 && !tokenIs<Operator>(pos - 1))
        {
            terms.push_back(new TokenRange(pos, pos + 1, getToken(pos)));

            TokenRange* range = parseTerms(pos + 1);

            terms.push_back(range);

            pos = range->end;
        }

        else if (tokenIs<OpenParenthesis>(pos))
        {
            const unsigned int pStart = pos;

            TokenRange* range = parseTerms(pos + 1);

            terms.push_back(range);

            pos = range->end;

            if (!tokenIs<CloseParenthesis>(pos))
            {
                tokenError(getToken(pos), "\")\"");

                return nullptr;
            }

            pos++;
        }

        else
        {
            TokenRange* range = parseTerm(pos);

            terms.push_back(range);

            pos = range->end;
        }
    } while (tokenIs<Operator>(pos));

    bool comparison = false;

    for (unsigned int i = 1; i < terms.size() - 1; i++)
    {
        if (dynamic_cast<const DoubleEqualToken*>(terms[i]->token))
        {
            if (comparison)
            {
                Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->token->line, terms[i]->token->character);
            }

            comparison = true;

            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new Equal(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + 1, terms.begin() + i + 2);

            i--;
        }

        else if (dynamic_cast<const LessToken*>(terms[i]->token))
        {
            if (comparison)
            {
                Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->token->line, terms[i]->token->character);
            }

            comparison = true;

            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new Less(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + 1, terms.begin() + i + 2);

            i--;
        }

        else if (dynamic_cast<const GreaterToken*>(terms[i]->token))
        {
            if (comparison)
            {
                Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->token->line, terms[i]->token->character);
            }

            comparison = true;
            
            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new Greater(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + 1, terms.begin() + i + 2);

            i--;
        }

        else if (dynamic_cast<const LessEqualToken*>(terms[i]->token))
        {
            if (comparison)
            {
                Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->token->line, terms[i]->token->character);
            }

            comparison = true;

            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new LessEqual(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + 1, terms.begin() + i + 2);

            i--;
        }

        else if (dynamic_cast<const GreaterEqualToken*>(terms[i]->token))
        {
            if (comparison)
            {
                Utils::parseError("Chaining comparison operators is not allowed.", path, terms[i]->token->line, terms[i]->token->character);
            }

            comparison = true;
            
            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new GreaterEqual(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + 1, terms.begin() + i + 2);

            i--;
        }
    }

    for (unsigned int i = 1; i < terms.size() - 1; i++)
    {
        if (dynamic_cast<const MultiplyToken*>(terms[i]->token))
        {
            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new Multiply(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + i, terms.begin() + i + 2);

            i--;
        }

        else if (dynamic_cast<const DivideToken*>(terms[i]->token))
        {
            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new Divide(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + i, terms.begin() + i + 2);

            i--;
        }
    }

    for (unsigned int i = 1; i < terms.size() - 1; i++)
    {
        if (dynamic_cast<const AddToken*>(terms[i]->token))
        {
            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new Add(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + i, terms.begin() + i + 2);

            i--;
        }

        else if (dynamic_cast<const SubtractToken*>(terms[i]->token))
        {
            terms[i - 1] = new TokenRange(terms[i - 1]->start, terms[i + 1]->end, new Subtract(terms[i - 1]->token, terms[i + 1]->token));

            terms.erase(terms.begin() + i, terms.begin() + i + 2);

            i--;
        }
    }

    return new TokenRange(start, pos, terms[0]->token);
}

TokenRange* Parser::parseTerm(unsigned int pos) const
{
    if (tokenIs<Name>(pos))
    {
        if (tokenIs<OpenParenthesis>(pos + 1))
        {
            return parseCall(pos);
        }

        return new TokenRange(pos, pos + 1, getToken(pos));
    }

    else if (tokenIs<Constant>(pos))
    {
        return new TokenRange(pos, pos + 1, getToken(pos));
    }

    else if (tokenIs<SubtractToken>(pos) && tokenIs<Constant>(pos + 1))
    {
        const Token* start = getToken(pos);

        return new TokenRange(pos, pos + 2, new Constant(start->line, start->character, "-" + getToken<Constant>(pos + 1)->string()));
    }

    tokenError(getToken(pos), "expression term");

    return nullptr;
}
