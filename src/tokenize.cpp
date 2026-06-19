#include "../include/tokenize.h"

using namespace Parser;

TokenListNode::TokenListNode(Token* token, TokenListNode* prev, TokenListNode* next, const bool end) :
    token(token), prev(prev), next(next), end(end) {}

TokenList::TokenList(const SourceProvider* source)
{
    head = new TokenListNode(new Token(SourceLocation(source, 0, 0)), nullptr, nullptr, true);
    tail = new TokenListNode(new Token(SourceLocation(source, 0, 0)), nullptr, nullptr, true);

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

Tokenizer::Tokenizer(const SourceProvider* source) :
    source(source), utils(Utils::get()) {}

TokenList* Tokenizer::tokenize()
{
    TokenList* tokens = new TokenList(source);

    while (current < source->length())
    {
        skipWhitespace();

        if (source->get(current) == '/' && current < source->length() - 1 && source->get(current + 1) == '/')
        {
            while (current < source->length() && source->get(current) != '\n')
            {
                current++;
            }
        }

        else if (source->get(current) == '/' && current < source->length() - 1 && source->get(current + 1) == '*')
        {
            current++;
            current++;

            while (current < source->length() - 1 && !(source->get(current) == '*' && source->get(current + 1) == '/'))
            {
                current++;
            }

            current++;
            current++;
        }

        else if (source->get(current) == '(')
        {
            tokens->add(new OpenParenthesis(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ')')
        {
            tokens->add(new CloseParenthesis(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '[')
        {
            tokens->add(new OpenSquareBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ']')
        {
            tokens->add(new CloseSquareBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '{')
        {
            tokens->add(new OpenCurlyBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '}')
        {
            tokens->add(new CloseCurlyBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ':')
        {
            tokens->add(new Colon(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ',')
        {
            tokens->add(new Comma(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '=')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens->add(new DoubleEquals(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens->add(new Equals(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '+')
        {
            tokens->add(new Add(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '-')
        {
            if (current < source->length() - 1 && isdigit(source->get(current + 1)) && !tokens->tail->prev->getToken<Identifier>() && !tokens->tail->prev->getToken<Value>())
            {
                tokens->add(tokenizeNumber());
            }

            else
            {
                tokens->add(new Subtract(SourceLocation(source, current, current + 1)));

                current++;
            }
        }

        else if (source->get(current) == '*')
        {
            tokens->add(new Multiply(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '/')
        {
            tokens->add(new Divide(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '^')
        {
            tokens->add(new Power(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '<')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens->add(new LessEqual(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens->add(new Less(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '>')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens->add(new GreaterEqual(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens->add(new Greater(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '"')
        {
            tokens->add(tokenizeString());
        }

        else if (isdigit(source->get(current)))
        {
            tokens->add(tokenizeNumber());
        }

        else if (isalpha(source->get(current)) || source->get(current) == '_')
        {
            tokens->add(tokenizeIdentifier());
        }

        else
        {
            throw OrganicParseException("Unrecognized symbol \"" + source->get(current, 1) + "\".", SourceLocation(source, current, current + 1));
        }

        skipWhitespace();
    }

    tokens->tail->token = new Token(SourceLocation(source, current, current));

    return tokens;
}

Token* Tokenizer::tokenizeString()
{
    const size_t start = current;

    current++;

    std::string str;

    while (current < source->length() && source->get(current) != '"' && source->get(current) != '\n')
    {
        str += source->get(current);

        current++;
    }

    if (current >= source->length() || source->get(current) != '"')
    {
        throw OrganicParseException("Expected closing double quotation mark.", SourceLocation(source, current, current));
    }

    current++;

    return new String(SourceLocation(source, start, current), str);
}

Token* Tokenizer::tokenizeNumber()
{
    const size_t start = current;

    std::string constant;

    if (source->get(current) == '-')
    {
        constant += '-';

        current++;
    }

    bool period = false;

    while (current < source->length() && (isdigit(source->get(current)) || source->get(current) == '.'))
    {
        if (source->get(current) == '.')
        {
            if (period)
            {
                throw OrganicParseException("Numbers cannot contain more than one decimal point.", SourceLocation(source, current, current + 1));
            }

            period = true;
        }

        constant += source->get(current);

        current++;
    }

    if (constant[constant.size() - 1] == '.')
    {
        throw OrganicParseException("Expected digits after decimal point.", SourceLocation(source, current, current));
    }

    return new Value(SourceLocation(source, start, current), std::stod(constant));
}

Token* Tokenizer::tokenizeIdentifier()
{
    const size_t start = current;

    std::string name;

    while (current < source->length() && (isalnum(source->get(current)) || source->get(current) == '-' || source->get(current) == '_'))
    {
        name += source->get(current);

        current++;
    }

    const SourceLocation location(source, start, current);

    if (name == "sequence-forward")
    {
        return new SequenceForward(location);
    }

    if (name == "sequence-backward")
    {
        return new SequenceBackward(location);
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
        return new Value(location, utils->pi);
    }

    if (name == "tau")
    {
        return new Value(location, utils->twoPi);
    }

    if (name == "e")
    {
        return new Value(location, utils->e);
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
            return new Value(location, getFrequency(base + 12 * (name[1] - 48)));
        }

        if (name.size() == 3 && isdigit(name[2]))
        {
            if (name[1] == 's')
            {
                return new Value(location, getFrequency(base + 12 * (name[2] - 48) + 1));
            }

            if (name[1] == 'f')
            {
                return new Value(location, getFrequency(base + 12 * (name[2] - 48) - 1));
            }
        }
    }

    return new Identifier(location);
}

void Tokenizer::skipWhitespace()
{
    while (current < source->length() && isspace(source->get(current)))
    {
        current++;
    }
}

double Tokenizer::getFrequency(const double note) const
{
    return 440 * pow(2, (note - 57) / 12);
}
