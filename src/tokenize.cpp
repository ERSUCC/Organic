#include "../include/tokenize.h"

namespace Parser
{
    TokenListNode::TokenListNode(Token* token, TokenListNode* prev, TokenListNode* next, const bool end) :
        token(token), prev(prev), next(next), end(end) {}

    TokenList::TokenList(const Path* path)
    {
        head = new TokenListNode(new Token(SourceLocation(path, 0, 0)), nullptr, nullptr, true);
        tail = new TokenListNode(new Token(SourceLocation(path, 0, 0)), nullptr, nullptr, true);

        head->prev = head;
        head->next = tail;

        tail->prev = head;
        tail->next = tail;
    }

    void TokenList::add(Token* token)
    {
        tail->prev->next = new TokenListNode(token, tail->prev, tail, false);
        tail->prev = tail->prev->next;
    }

    TokenListNode* TokenList::stitch(TokenListNode* start, TokenListNode* end)
    {
        start->prev->next = end;
        end->prev = start->prev;

        return end;
    }

    TokenListNode* TokenList::patch(TokenListNode* start, TokenListNode* end, Token* token)
    {
        start->prev->next = new TokenListNode(token, start->prev, end, false);
        end->prev = start->prev->next;

        return end;
    }

    Tokenizer::Tokenizer(const Path* path) :
        path(path)
    {
        utils = Utils::get();

        if (!path->readToString(code))
        {
            throw OrganicFileException("Could not open \"" + path->string() + "\".");
        }
    }

    TokenList* Tokenizer::tokenize()
    {
        TokenList* tokens = new TokenList(path);

        while (current < code.size())
        {
            skipWhitespace();

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
                tokens->add(new OpenParenthesis(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == ')')
            {
                tokens->add(new CloseParenthesis(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == '[')
            {
                tokens->add(new OpenSquareBracket(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == ']')
            {
                tokens->add(new CloseSquareBracket(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == '{')
            {
                tokens->add(new OpenCurlyBracket(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == '}')
            {
                tokens->add(new CloseCurlyBracket(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == ':')
            {
                tokens->add(new Colon(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == ',')
            {
                tokens->add(new Comma(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == '=')
            {
                if (current < code.size() - 1 && code[current + 1] == '=')
                {
                    tokens->add(new DoubleEquals(SourceLocation(path, line, character)));

                    nextCharacter();
                }

                else
                {
                    tokens->add(new Equals(SourceLocation(path, line, line)));
                }

                nextCharacter();
            }

            else if (code[current] == '+')
            {
                tokens->add(new Add(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == '-')
            {
                if (current < code.size() - 1 && isdigit(code[current + 1]) && !tokens->tail->prev->getToken<Identifier>() && !tokens->tail->prev->getToken<Value>())
                {
                    tokens->add(tokenizeNumber());
                }

                else
                {
                    tokens->add(new Subtract(SourceLocation(path, line, character)));

                    nextCharacter();
                }
            }

            else if (code[current] == '*')
            {
                tokens->add(new Multiply(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == '/')
            {
                tokens->add(new Divide(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == '^')
            {
                tokens->add(new Power(SourceLocation(path, line, character)));

                nextCharacter();
            }

            else if (code[current] == '<')
            {
                if (current < code.size() - 1 && code[current] + 1 == '=')
                {
                    tokens->add(new LessEqual(SourceLocation(path, line, character)));

                    nextCharacter();
                }

                else
                {
                    tokens->add(new Less(SourceLocation(path, line, character)));
                }

                nextCharacter();
            }

            else if (code[current] == '>')
            {
                if (current < code.size() - 1 && code[current + 1] == '=')
                {
                    tokens->add(new GreaterEqual(SourceLocation(path, line, character)));

                    nextCharacter();
                }

                else
                {
                    tokens->add(new Greater(SourceLocation(path, line, character)));
                }

                nextCharacter();
            }

            else if (code[current] == '"')
            {
                tokens->add(tokenizeString());
            }

            else if (isdigit(code[current]))
            {
                tokens->add(tokenizeNumber());
            }

            else if (isalpha(code[current]) || code[current] == '_')
            {
                tokens->add(tokenizeIdentifier());
            }

            else
            {
                throw OrganicParseException("Unrecognized symbol \"" + std::string(1, code[current]) + "\".", SourceLocation(path, line, character));
            }

            skipWhitespace();
        }

        tokens->tail->token = new Token(SourceLocation(path, line, character));

        return tokens;
    }

    Token* Tokenizer::tokenizeString()
    {
        const SourceLocation location = SourceLocation(path, line, character);

        nextCharacter();

        std::string str;

        while (current < code.size() && code[current] != '"')
        {
            str += code[current];

            nextCharacter();
        }

        if (current >= code.size() || code[current] != '"')
        {
            throw OrganicParseException("Unexpected end of file.", SourceLocation(path, line, character));
        }

        nextCharacter();

        return new String(location, str);
    }

    Token* Tokenizer::tokenizeNumber()
    {
        const SourceLocation location = SourceLocation(path, line, character);

        std::string constant;

        if (code[current] == '-')
        {
            constant += '-';

            nextCharacter();
        }

        bool period = false;

        while (current < code.size() && (isdigit(code[current]) || code[current] == '.'))
        {
            if (code[current] == '.')
            {
                if (period)
                {
                    throw OrganicParseException("Numbers cannot contain more than one decimal point.", SourceLocation(path, line, character));
                }

                period = true;
            }

            constant += code[current];

            nextCharacter();
        }

        return new Value(location, constant, std::stod(constant));
    }

    Token* Tokenizer::tokenizeIdentifier()
    {
        const SourceLocation location(path, line, character);

        std::string name;

        while (current < code.size() && (isalnum(code[current]) || code[current] == '-' || code[current] == '_'))
        {
            name += code[current];

            nextCharacter();
        }

        if (name == "sequence-forwards")
        {
            return new SequenceForwards(location);
        }

        if (name == "sequence-backwards")
        {
            return new SequenceBackwards(location);
        }

        if (name == "sequence-ping-pong")
        {
            return new SequencePingPong(location);
        }

        if (name == "sequence-random")
        {
            return new SequenceRandom(location);
        }

        if (name == "random-step")
        {
            return new RandomStep(location);
        }

        if (name == "random-linear")
        {
            return new RandomLinear(location);
        }

        if (name == "round-nearest")
        {
            return new RoundNearest(location);
        }

        if (name == "round-up")
        {
            return new RoundUp(location);
        }

        if (name == "round-down")
        {
            return new RoundDown(location);
        }

        if (name == "pi")
        {
            return new Value(location, "pi", utils->pi);
        }

        if (name == "e")
        {
            return new Value(location, "e", utils->e);
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
                return new Value(location, name, getFrequency(base + 12 * (name[1] - 48)));
            }

            if (name.size() == 3 && isdigit(name[2]))
            {
                if (name[1] == 's')
                {
                    return new Value(location, name, getFrequency(base + 12 * (name[2] - 48) + 1));
                }

                if (name[1] == 'f')
                {
                    return new Value(location, name, getFrequency(base + 12 * (name[2] - 48) - 1));
                }
            }
        }

        return new Identifier(location, name);
    }

    void Tokenizer::skipWhitespace()
    {
        while (current < code.size() && isspace(code[current]))
        {
            nextCharacter();
        }
    }

    void Tokenizer::nextCharacter()
    {
        if (code[current] == '\n')
        {
            line++;
            character = 0;
        }

        current++;
        character++;
    }

    double Tokenizer::getFrequency(const double note) const
    {
        return 440 * pow(2, (note - 57) / 12);
    }
}
