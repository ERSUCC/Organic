#include "../include/tokenize.h"

namespace Parser
{
    Tokenizer::Tokenizer(const Path* path) :
        path(path)
    {
        utils = Utils::get();

        if (!path->readToString(code))
        {
            throw OrganicFileException("Could not open \"" + path->string() + "\".");
        }
    }

    std::vector<BasicToken*> Tokenizer::tokenize()
    {
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
                tokens.push_back(new OpenParenthesis(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ')')
            {
                tokens.push_back(new CloseParenthesis(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '[')
            {
                tokens.push_back(new OpenSquareBracket(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ']')
            {
                tokens.push_back(new CloseSquareBracket(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '{')
            {
                tokens.push_back(new OpenCurlyBracket(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '}')
            {
                tokens.push_back(new CloseCurlyBracket(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ':')
            {
                tokens.push_back(new Colon(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == ',')
            {
                tokens.push_back(new Comma(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '=')
            {
                unsigned int startLine = line;
                unsigned int startCharacter = character;

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
                tokens.push_back(new Add(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '-')
            {
                tokens.push_back(new Subtract(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '*')
            {
                tokens.push_back(new Multiply(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '/')
            {
                tokens.push_back(new Divide(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '^')
            {
                tokens.push_back(new Power(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1)));

                nextCharacter();
            }

            else if (code[current] == '<')
            {
                unsigned int startLine = line;
                unsigned int startCharacter = character;

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
                unsigned int startLine = line;
                unsigned int startCharacter = character;

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
                tokenizeString(line, character);
            }

            else if (isdigit(code[current]))
            {
                tokenizeNumber(line, character);
            }

            else if (isalpha(code[current]) || code[current] == '_')
            {
                tokenizeIdentifier(line, character);
            }

            else
            {
                throw OrganicParseException("Unrecognized symbol \"" + std::string(1, code[current]) + "\".", SourceLocation(path, line, character, 0, 0));
            }

            skipWhitespace();
        }

        tokens.push_back(new BasicToken(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1), ""));

        return tokens;
    }

    void Tokenizer::tokenizeString(const unsigned int line, const unsigned int character)
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
            throw OrganicParseException("Unexpected end of file.", SourceLocation(path, this->line, this->character, 0, 0));
        }

        tokens.push_back(new String(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1), str));

        nextCharacter();
    }

    void Tokenizer::tokenizeNumber(const unsigned int line, const unsigned int character)
    {
        std::string constant;

        bool period = false;

        while (current < code.size() && (isdigit(code[current]) || code[current] == '.'))
        {
            if (code[current] == '.')
            {
                if (period)
                {
                    throw OrganicParseException("Numbers cannot contain more than one decimal point.", SourceLocation(path, line, character, 0, 0));
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
            tokens.push_back(new Value(SourceLocation(path, line, character, tokens.size(), tokens.size() + 1), constant, std::stod(constant)));
        }
    }

    void Tokenizer::tokenizeIdentifier(const unsigned int line, const unsigned int character)
    {
        unsigned int startLine = line;
        unsigned int startCharacter = character;

        std::string name;

        while (current < code.size() && (isalnum(code[current]) || code[current] == '-' || code[current] == '_'))
        {
            name += code[current];

            nextCharacter();
        }

        const SourceLocation location(path, startLine, startCharacter, tokens.size(), tokens.size() + 1);

        if (name == "sequence-forwards")
        {
            tokens.push_back(new SequenceForwards(location));
        }

        else if (name == "sequence-backwards")
        {
            tokens.push_back(new SequenceBackwards(location));
        }

        else if (name == "sequence-ping-pong")
        {
            tokens.push_back(new SequencePingPong(location));
        }

        else if (name == "sequence-random")
        {
            tokens.push_back(new SequenceRandom(location));
        }

        else if (name == "random-step")
        {
            tokens.push_back(new RandomStep(location));
        }

        else if (name == "random-linear")
        {
            tokens.push_back(new RandomLinear(location));
        }

        else if (name == "round-nearest")
        {
            tokens.push_back(new RoundNearest(location));
        }

        else if (name == "round-up")
        {
            tokens.push_back(new RoundUp(location));
        }

        else if (name == "round-down")
        {
            tokens.push_back(new RoundDown(location));
        }

        else if (name == "pi")
        {
            tokens.push_back(new Value(location, "pi", utils->pi));
        }

        else if (name == "e")
        {
            tokens.push_back(new Value(location, "e", utils->e));
        }

        else
        {
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
                    tokens.push_back(new Value(location, name, getFrequency(base + 12 * (name[1] - 48))));
                }

                else if (name.size() == 3 && isdigit(name[2]))
                {
                    if (name[1] == 's')
                    {
                        tokens.push_back(new Value(location, name, getFrequency(base + 12 * (name[2] - 48) + 1)));
                    }

                    else if (name[1] == 'f')
                    {
                        tokens.push_back(new Value(location, name, getFrequency(base + 12 * (name[2] - 48) - 1)));
                    }

                    else
                    {
                        tokens.push_back(new Identifier(location, name));
                    }
                }

                else
                {
                    tokens.push_back(new Identifier(location, name));
                }
            }

            else
            {
                tokens.push_back(new Identifier(location, name));
            }
        }
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

    BasicToken* Tokenizer::getToken(const unsigned int pos) const
    {
        if (pos >= tokens.size() - 1)
        {
            throw OrganicParseException("Unexpected end of file.", tokens.back()->location);
        }

        return tokens[pos];
    }

    template <typename T> bool Tokenizer::tokenIs(const unsigned int pos) const
    {
        if (pos >= tokens.size() - 1)
        {
            return false;
        }

        return dynamic_cast<T*>(getToken(pos));
    }
}
