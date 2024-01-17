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
        TokenRange* range = parseInstruction(current);

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

        int startLine = line;
        int startCharacter = character;

        if (code[current] == '/' && current < code.size() - 1 && code[current + 1] == '/')
        {
            while (current < code.size() && code[current] != '\n')
            {
                nextCharacter();
            }
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
            tokens.push_back(new Equals(startLine, startCharacter));

            nextCharacter();
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
                        Utils::parseError("Constants must contain at most one decimal point.", path, startLine, startCharacter);
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
                    Utils::parseError("\"#\" can only precede variable names.", path, line, character);
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

Token* Parser::getToken(const int pos)
{
    if (pos >= tokens.size())
    {
        Utils::parseError("Unexpected end of file.", path, line, character);

        return nullptr;
    }

    return tokens[pos];
}

template <typename T> T* Parser::getToken(const int pos)
{
    return dynamic_cast<T*>(getToken(pos));
}

template <typename T> bool Parser::tokenIs(const int pos)
{
    if (pos >= tokens.size())
    {
        return false;
    }

    return dynamic_cast<T*>(getToken(pos));
}

void Parser::tokenError(const Token* token, const std::string expected)
{
    Utils::parseError("Expected " + expected + ", received \"" + token->string() + "\".", path, token->line, token->character);
}

TokenRange* Parser::parseInstruction(int pos)
{
    if (!tokenIs<Name>(pos))
    {
        tokenError(getToken(pos), "name");
    }

    if (tokenIs<Equals>(pos + 1))
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

TokenRange* Parser::parseAssign(int pos)
{
    TokenRange* range = parseExpression(pos + 2);

    return new TokenRange(pos, range->end, new Assign(getToken<Name>(pos), range->token));
}

TokenRange* Parser::parseCall(int pos)
{
    int start = pos;

    pos += 2;

    std::vector<Argument*> arguments;

    if (tokenIs<Name>(pos))
    {
        pos -= 1;

        do
        {
            TokenRange* range = parseArgument(pos + 1);

            arguments.push_back((Argument*)range->token);

            pos = range->end;
        } while (tokenIs<Comma>(pos));
    }

    if (tokenIs<CloseParenthesis>(pos))
    {
        Name* name = getToken<Name>(start);

        return new TokenRange(start, pos + 1, new Call(name->line, name->character, name->name, arguments));
    }

    tokenError(getToken(pos), "\")\"");

    return nullptr;
}

TokenRange* Parser::parseArgument(int pos)
{
    if (!tokenIs<Name>(pos))
    {
        tokenError(getToken(pos), "input name");
    }

    if (!tokenIs<Colon>(pos + 1))
    {
        tokenError(getToken(pos + 1), "\":\"");
    }

    TokenRange* range = parseExpression(pos + 2);

    Name* name = getToken<Name>(pos);

    return new TokenRange(pos, range->end, new Argument(name->line, name->character, name->name, range->token));
}

TokenRange* Parser::parseExpression(int pos)
{
    if (tokenIs<OpenParenthesis>(pos) && tokenIs<Comma>(parseExpression(pos + 1)->end))
    {
        int start = pos;

        std::vector<Token*> list;

        do
        {
            TokenRange* range = parseExpression(pos + 1);

            list.push_back(range->token);

            pos = range->end;
        } while (tokenIs<Comma>(pos));

        if (!tokenIs<CloseParenthesis>(pos))
        {
            tokenError(getToken(pos), "\")\"");
        }

        Token* token = getToken(start);

        if (list.size() == 0)
        {
            tokenError(token, "expression");
        }

        return new TokenRange(start, pos + 1, new List(token->line, token->character, list));
    }

    return parseTerms(pos);
}

TokenRange* Parser::parseTerms(int pos)
{
    int start = pos;

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
            int pStart = pos;

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

    for (int i = 1; i < terms.size() - 1; i++)
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

    for (int i = 1; i < terms.size() - 1; i++)
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

TokenRange* Parser::parseTerm(int pos)
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
        Token* start = getToken(pos);

        return new TokenRange(pos, pos + 2, new Constant(start->line, start->character, "-" + getToken<Constant>(pos + 1)->string()));
    }

    tokenError(getToken(pos), "expression term");

    return nullptr;
}
