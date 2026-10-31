#include "tokenize.h"

using namespace Parser;

#define CALL(func, ...) [](const SourceLocation& location) { return new func(location, __VA_ARGS__); }

static std::unordered_map<std::string, std::function<const Token* (const SourceLocation&)>> constants =
{
    { "anything", CALL(TypeName, new AnyType()) },
    { "number", CALL(TypeName, new NumberType()) },
    { "boolean", CALL(TypeName, new BooleanType()) },
    { "sequence-order", CALL(TypeName, new SequenceOrderType()) },
    { "random-type", CALL(TypeName, new RandomTypeType()) },
    { "round-direction", CALL(TypeName, new RoundDirectionType()) },
    { "string", CALL(TypeName, new StringType()) },
    { "audio-source", CALL(TypeName, new AudioSourceType()) },
    { "effect", CALL(TypeName, new EffectType()) },
    { "forward", CALL(Constant, new SequenceOrderType(), Constants::Sequence::Forward) },
    { "backward", CALL(Constant, new SequenceOrderType(), Constants::Sequence::Backward) },
    { "shuffle", CALL(Constant, new SequenceOrderType(), Constants::Sequence::Shuffle) },
    { "stay", CALL(Constant, new RandomTypeType(), Constants::Random::Stay) },
    { "linear", CALL(Constant, new RandomTypeType(), Constants::Random::Linear) },
    { "nearest", CALL(Constant, new RoundDirectionType(), Constants::Round::Nearest) },
    { "up", CALL(Constant, new RoundDirectionType(), Constants::Round::Up) },
    { "down", CALL(Constant, new RoundDirectionType(), Constants::Round::Down) },
    { "true", CALL(Boolean, true) },
    { "false", CALL(Boolean, false) },
    { "pi", CALL(Value, Utils::pi) },
    { "tau", CALL(Value, Utils::twoPi) },
    { "e", CALL(Value, Utils::e) }
};

OrganicTokenException::OrganicTokenException(const Token* token, const std::string& expected) :
    OrganicParseException(getMessage(token, expected), token->location) {}

std::string OrganicTokenException::getMessage(const Token* token, const std::string& expected)
{
    if (token->eof())
    {
        return "Expected " + expected + ".";
    }

    return "Expected " + expected + ", but received \"" + token->string() + "\".";
}

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

    try
    {
        TokenIterator* tokens = tokenizer->tokenizeProgram();

        delete tokenizer;

        return tokens;
    }

    catch (const OrganicException& e)
    {
        delete tokenizer;

        throw;
    }
}

Tokenizer::Tokenizer(const SourceProvider* source) :
    source(source), utils(Utils::get()) {}

TokenIterator* Tokenizer::tokenizeProgram()
{
    TokenIterator* tokens = new TokenIterator();

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
            tokens->push(new OpenParenthesis(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ')')
        {
            tokens->push(new CloseParenthesis(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '[')
        {
            tokens->push(new OpenSquareBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ']')
        {
            tokens->push(new CloseSquareBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '{')
        {
            tokens->push(new OpenCurlyBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '}')
        {
            tokens->push(new CloseCurlyBracket(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ':')
        {
            tokens->push(new Colon(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == ',')
        {
            tokens->push(new Comma(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '=')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens->push(new DoubleEquals(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens->push(new Equals(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '+')
        {
            tokens->push(new Add(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '-')
        {
            if (current < source->length() - 1 && isdigit(source->get(current + 1)) && !tokens->last<Identifier>() && !tokens->last<Value>())
            {
                try
                {
                    tokens->push(tokenizeNumber());
                }

                catch (const OrganicException& e)
                {
                    delete tokens;

                    throw;
                }
            }

            else
            {
                tokens->push(new Subtract(SourceLocation(source, current, current + 1)));

                current++;
            }
        }

        else if (source->get(current) == '*')
        {
            tokens->push(new Multiply(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '/')
        {
            tokens->push(new Divide(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '^')
        {
            tokens->push(new Power(SourceLocation(source, current, current + 1)));

            current++;
        }

        else if (source->get(current) == '<')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens->push(new LessEqual(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens->push(new Less(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '>')
        {
            if (current < source->length() - 1 && source->get(current + 1) == '=')
            {
                tokens->push(new GreaterEqual(SourceLocation(source, current, current + 2)));

                current++;
            }

            else
            {
                tokens->push(new Greater(SourceLocation(source, current, current + 1)));
            }

            current++;
        }

        else if (source->get(current) == '"')
        {
            try
            {
                tokens->push(tokenizeString());
            }

            catch (const OrganicException& e)
            {
                delete tokens;

                throw;
            }
        }

        else if (isdigit(source->get(current)))
        {
            try
            {
                tokens->push(tokenizeNumber());
            }

            catch (const OrganicException& e)
            {
                delete tokens;

                throw;
            }
        }

        else if (isalpha(source->get(current)) || source->get(current) == '_')
        {
            tokens->push(tokenizeIdentifier());
        }

        else if (source->get(current) == '|')
        {
            try
            {
                tokens->push(tokenizeFillable());
            }

            catch (const OrganicException& e)
            {
                delete tokens;

                throw;
            }
        }

        else
        {
            delete tokens;

            throw OrganicParseException("Unrecognized symbol \"" + source->get(current, 1) + "\".", SourceLocation(source, current, current + 1));
        }

        skipWhitespace();
    }

    tokens->push(new Eof(SourceLocation(source, source->length(), source->length())));

    return tokens;
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
        throw OrganicParseException("Expected a closing double quotation mark.", SourceLocation(source, current, current));
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
        throw OrganicParseException("Expected digits after a decimal point.", SourceLocation(source, current, current));
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

    if (constants.count(name))
    {
        return constants.at(name)(location);
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

const Token* Tokenizer::tokenizeFillable()
{
    const size_t start = current++;

    std::string name;

    while (current < source->length() && (isalnum(source->get(current)) || source->get(current) == '-' || source->get(current) == '_'))
    {
        name += source->get(current++);
    }

    if (source->get(current) != '|')
    {
        throw OrganicParseException("Expected \"|\" after a fillable value name.", SourceLocation(source, current, current));
    }

    return new Fillable(SourceLocation(source, start, ++current), name);
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
