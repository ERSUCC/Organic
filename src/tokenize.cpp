#include "../include/tokenize.h"

using namespace Parser;

OrganicTokenException::OrganicTokenException(const Token* token, const std::string& expected) :
    OrganicParseException(getMessage(token, expected), token->location), token(token), expected(expected) {}

std::string OrganicTokenException::getMessage(const Token* token, const std::string& expected)
{
    if (token->eof())
    {
        return "Expected " + expected + ".";
    }

    return "Expected " + expected + ", but received \"" + token->string() + "\".";
}

TokenIterator::TokenIterator(const std::vector<const Token*>& tokens) :
    tokens(tokens) {}

TokenIterator::~TokenIterator()
{
    while (current < tokens.size())
    {
        delete tokens[current++];
    }
}

const Token* TokenIterator::peek(const size_t offset) const
{
    if (current + offset < tokens.size())
    {
        return tokens[current + offset];
    }

    return tokens.back();
}

TokenIterator* TokenIterator::drop(const size_t count)
{
    for (size_t i = 0; i < count && current < tokens.size() - 1; i++)
    {
        delete tokens[current];

        tokens[current++] = nullptr;
    }

    return this;
}

TokenIterator* Tokenizer::tokenize(const SourceProvider* source)
{
    Tokenizer* tokenizer = new Tokenizer(source);

    TokenIterator* tokens = tokenizer->tokenizeProgram();

    delete tokenizer;

    return tokens;
}

Tokenizer::Tokenizer(const SourceProvider* source) :
    source(source), utils(Utils::get()) {}

TokenIterator* Tokenizer::tokenizeProgram()
{
    std::vector<const Token*> tokens;

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
            tokens.push_back(new OpenParenthesis(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ')')
        {
            tokens.push_back(new CloseParenthesis(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '[')
        {
            tokens.push_back(new OpenSquareBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ']')
        {
            tokens.push_back(new CloseSquareBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '{')
        {
            tokens.push_back(new OpenCurlyBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '}')
        {
            tokens.push_back(new CloseCurlyBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ':')
        {
            tokens.push_back(new Colon(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ',')
        {
            tokens.push_back(new Comma(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '=')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens.push_back(new DoubleEquals(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens.push_back(new Equals(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '+')
        {
            tokens.push_back(new Add(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '-')
        {
            if (current < source->length() - 1 && isdigit(source->get(current + 1)) && !tokens.empty() && !dynamic_cast<const Identifier*>(tokens.back()) && !dynamic_cast<const Value*>(tokens.back()))
            {
                tokens.push_back(tokenizeNumber());
            }

            else
            {
                tokens.push_back(new Subtract(SourceLocation(source, current, current + 1)));

                current++;
            }
        }

        else if (source->get(current) == '*')
        {
            tokens.push_back(new Multiply(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '/')
        {
            tokens.push_back(new Divide(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '^')
        {
            tokens.push_back(new Power(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '<')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens.push_back(new LessEqual(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens.push_back(new Less(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '>')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens.push_back(new GreaterEqual(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens.push_back(new Greater(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '"')
        {
            tokens.push_back(tokenizeString());
        }

        else if (isdigit(source->get(current)))
        {
            tokens.push_back(tokenizeNumber());
        }

        else if (isalpha(source->get(current)) || source->get(current) == '_')
        {
            tokens.push_back(tokenizeIdentifier());
        }

        else
        {
            throw OrganicParseException("Unrecognized symbol \"" + source->get(current, 1) + "\".", SourceLocation(source, current, current + 1));
        }

        skipWhitespace();
    }

    tokens.push_back(new Eof(SourceLocation(source, source->length(), source->length())));

    return new TokenIterator(tokens);
}

const Token* Tokenizer::tokenizeString()
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

const Token* Tokenizer::tokenizeNumber()
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

const Token* Tokenizer::tokenizeIdentifier()
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
        return new Constant(location, new SequenceOrderType(), Constants::Sequence::Forward);
    }

    if (name == "sequence-backward")
    {
        return new Constant(location, new SequenceOrderType(), Constants::Sequence::Backward);
    }

    if (name == "sequence-ping-pong")
    {
        return new Constant(location, new SequenceOrderType(), Constants::Sequence::PingPong);
    }

    if (name == "sequence-random")
    {
        return new Constant(location, new SequenceOrderType(), Constants::Sequence::Random);
    }

    if (name == "random-step")
    {
        return new Constant(location, new RandomTypeType(), Constants::Random::Step);
    }

    if (name == "random-linear")
    {
        return new Constant(location, new RandomTypeType(), Constants::Random::Linear);
    }

    if (name == "round-nearest")
    {
        return new Constant(location, new RoundDirectionType(), Constants::Round::Nearest);
    }

    if (name == "round-up")
    {
        return new Constant(location, new RoundDirectionType(), Constants::Round::Up);
    }

    if (name == "round-down")
    {
        return new Constant(location, new RoundDirectionType(), Constants::Round::Down);
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
