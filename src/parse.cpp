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
    program = new Program();

    while (pos < code.size())
    {
        parseInstruction();
        skipWhitespace();
    }

    return program;
}

Token* Parser::getToken()
{
    Token* token = tokens.top();

    tokens.pop();

    return token;
}

template <typename T> T* Parser::getToken()
{
    return dynamic_cast<T*>(getToken());
}

template <typename T> bool Parser::nextTokenIs()
{
    return dynamic_cast<T*>(tokens.top());
}

void Parser::skipWhitespace()
{
    while (pos < code.size() && isspace(code[pos]))
    {
        nextCharacter();
    }
}

void Parser::nextCharacter()
{
    if (code[pos] == '\n')
    {
        line++;
        character = 0;
    }

    pos++;
    character++;
}

void Parser::parseInstruction()
{
    skipWhitespace();

    if (code[pos] == '#')
    {
        parseComment();
    }

    else
    {
        parseName();
        skipWhitespace();

        if (code[pos] == '(')
        {
            parseCall();
        }

        else
        {
            parseAssign();
        }

        program->instructions.push_back(getToken<Instruction>());
    }
}

void Parser::parseComment()
{
    skipWhitespace();
    parseSingleChar('#');

    while (pos < code.size() && code[pos] != '\n')
    {
        nextCharacter();
    }
}

void Parser::parseAssign()
{
    if (!nextTokenIs<Name>())
    {
        Token* token = getToken();

        Utils::parseError("Cannot define a variable with a reserved name.", path, token->line, token->character);
    }

    Name* name = getToken<Name>();

    skipWhitespace();
    parseSingleChar('=');
    parseExpression();

    tokens.push(new Assign(name->line, name->character, name->name, getToken()));
}

void Parser::parseExpression()
{
    skipWhitespace();

    int opens = 0;

    while (code[pos] == '(')
    {
        parseSingleChar('(');
        skipWhitespace();

        opens++;
    }

    if (isalpha(code[pos]) || code[pos] == '_')
    {
        parseName();
        skipWhitespace();

        if (code[pos] == '(')
        {
            parseCall();
        }

        else if (nextTokenIs<Name>())
        {
            Name* name = getToken<Name>();

            tokens.push(new VariableRef(name->line, name->character, name->name));
        }
    }

    else if (isdigit(code[pos]))
    {
        parseConstant();
    }

    else
    {
        Utils::parseError("Illegal start of expression.", path, line, character);
    }

    skipWhitespace();

    if (code[pos] == '+')
    {
        Token* value1 = getToken();

        parseSingleChar('+');
        skipWhitespace();
        parseExpression();

        tokens.push(new CreateValueAdd(value1->line, value1->character, value1, getToken()));
    }

    else if (code[pos] == '-')
    {
        Token* value1 = getToken();

        parseSingleChar('-');
        skipWhitespace();
        parseExpression();

        tokens.push(new CreateValueSubtract(value1->line, value1->character, value1, getToken()));
    }

    else if (code[pos] == '*')
    {
        Token* value1 = getToken();

        parseSingleChar('*');
        skipWhitespace();
        parseExpression();

        tokens.push(new CreateValueMultiply(value1->line, value1->character, value1, getToken()));
    }

    else if (code[pos] == '/')
    {
        Token* value1 = getToken();

        parseSingleChar('/');
        skipWhitespace();
        parseExpression();

        tokens.push(new CreateValueDivide(value1->line, value1->character, value1, getToken()));
    }

    else if (code[pos] == ',' && opens > 0)
    {
        Token* value1 = getToken();

        int n = 0;

        while (code[pos] == ',')
        {
            parseSingleChar(',');
            skipWhitespace();
            parseExpression();

            n++;
        }

        List* list = new List(value1->line, value1->character);

        list->items.push_back(value1);

        for (; n > 0; n--)
        {
            list->items.push_back(getToken());
        }

        tokens.push(list);
    }

    for (; opens > 0; opens--)
    {
        parseSingleChar(')');
    }
}

void Parser::parseCall()
{
    if (!nextTokenIs<Name>())
    {
        Token* token = getToken();
        
        Utils::parseError("Reserved names cannot be used as functions.", path, token->line, token->character);
    }

    Name* name = getToken<Name>();

    skipWhitespace();
    parseSingleChar('(');

    if (name->name == "sine" || name->name == "square" || name->name == "saw" || name->name == "triangle")
    {
        Token* volume = new Constant(name->line, name->character, 1);
        Token* pan = new Constant(name->line, name->character, 0);
        Token* frequency = new Constant(name->line, name->character, 0);
        List* effects = new List(name->line, name->character);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "volume")
            {
                volume = argument->value;
            }

            else if (argument->name->name == "pan")
            {
                pan = argument->value;
            }

            else if (argument->name->name == "frequency")
            {
                frequency = argument->value;
            }

            else if (argument->name->name == "effects")
            {
                effects = (List*)argument->value;
            }

            else
            {
                Utils::parseError("Unknown input name \"" + argument->name->name + "\".", path, argument->line, argument->character);
            }

            skipWhitespace();
        }

        if (name->name == "sine")
        {
            tokens.push(new CreateSine(name->line, name->character, volume, pan, frequency, effects));
        }
        
        else if (name->name == "square")
        {
            tokens.push(new CreateSquare(name->line, name->character, volume, pan, frequency, effects));
        }

        else if (name->name == "saw")
        {
            tokens.push(new CreateSaw(name->line, name->character, volume, pan, frequency, effects));
        }

        else if (name->name == "triangle")
        {
            tokens.push(new CreateTriangle(name->line, name->character, volume, pan, frequency, effects));
        }
    }

    else if (name->name == "hold")
    {
        Token* value = new Constant(name->line, name->character, 0);
        Token* length = new Constant(name->line, name->character, 0);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "value")
            {
                value = argument->value;
            }

            else if (argument->name->name == "length")
            {
                length = argument->value;
            }

            else
            {
                Utils::parseError("Unknown input name \"" + argument->name->name + "\".", path, argument->line, argument->character);
            }

            skipWhitespace();
        }

        tokens.push(new CreateHold(name->line, name->character, value, length));
    }

    else if (name->name == "lfo" || name->name == "sweep")
    {
        Token* from = new Constant(name->line, name->character, 0);
        Token* to = new Constant(name->line, name->character, 1);
        Token* length = new Constant(name->line, name->character, 0);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "from")
            {
                from = argument->value;
            }

            else if (argument->name->name == "to")
            {
                to = argument->value;
            }

            else if (argument->name->name == "length")
            {
                length = argument->value;
            }

            else
            {
                Utils::parseError("Unknown input name \"" + argument->name->name + "\".", path, argument->line, argument->character);
            }

            skipWhitespace();
        }

        if (name->name == "lfo")
        {
            tokens.push(new CreateLFO(name->line, name->character, from, to, length));
        }

        else if (name->name == "sweep")
        {
            tokens.push(new CreateSweep(name->line, name->character, from, to, length));
        }
    }

    else if (name->name == "sequence")
    {
        List* values = new List(name->line, name->character);
        Token* order = new GroupOrder(name->line, name->character, Sequence::OrderEnum::Forwards);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "values")
            {
                values = (List*)argument->value;
            }

            else if (argument->name->name == "order")
            {
                order = argument->value;
            }

            else
            {
                Utils::parseError("Unknown input name \"" + argument->name->name + "\".", path, argument->line, argument->character);
            }

            skipWhitespace();
        }

        tokens.push(new CreateSequence(name->line, name->character, values, order));
    }

    else if (name->name == "repeat")
    {
        Token* value = new Constant(name->line, name->character, 0);
        Token* repeats = new Constant(name->line, name->character, 0);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "value")
            {
                value = argument->value;
            }

            else if (argument->name->name == "repeats")
            {
                repeats = argument->value;
            }

            else
            {
                Utils::parseError("Unknown input name \"" + argument->name->name + "\".", path, argument->line, argument->character);
            }

            skipWhitespace();
        }

        tokens.push(new CreateRepeat(name->line, name->character, value, repeats));
    }

    else if (name->name == "random")
    {
        Token* from = new Constant(name->line, name->character, 0);
        Token* to = new Constant(name->line, name->character, 1);
        Token* length = new Constant(name->line, name->character, 0);
        RandomType* type = new RandomType(name->line, name->character, Random::TypeEnum::Step);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "from")
            {
                from = argument->value;
            }

            else if (argument->name->name == "to")
            {
                to = argument->value;
            }

            else if (argument->name->name == "length")
            {
                length = argument->value;
            }

            else if (argument->name->name == "type")
            {
                type = dynamic_cast<RandomType*>(argument->value);
            }

            else
            {
                Utils::parseError("Unknown input name \"" + argument->name->name + "\".", path, argument->line, argument->character);
            }

            skipWhitespace();
        }

        tokens.push(new CreateRandom(name->line, name->character, from, to, length, type));
    }

    else if (name->name == "delay")
    {
        Token* mix = new Constant(name->line, name->character, 1);
        Token* delay = new Constant(name->line, name->character, 0);
        Token* feedback = new Constant(name->line, name->character, 0);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "mix")
            {
                mix = argument->value;
            }

            else if (argument->name->name == "delay")
            {
                delay = argument->value;
            }

            else if (argument->name->name == "feedback")
            {
                feedback = argument->value;
            }

            else
            {
                Utils::parseError("Unknown input name \"" + argument->name->name + "\".", path, argument->line, argument->character);
            }

            skipWhitespace();
        }

        tokens.push(new CreateDelay(name->line, name->character, mix, delay, feedback));
    }

    else
    {
        Utils::parseError("Unknown function \"" + name->name + "\".", path, name->line, name->character);
    }

    parseSingleChar(')');
}

void Parser::parseArgument()
{
    skipWhitespace();
    parseName();

    Name* name = getToken<Name>();

    parseSingleChar(':');
    parseExpression();

    tokens.push(new Argument(name->line, name->character, name, getToken()));

    skipWhitespace();

    if (code[pos] != ')')
    {
        parseSingleChar(',');
        skipWhitespace();

        if (code[pos] == ')')
        {
            Utils::parseError("Expected expression, received \")\".", path, line, character);
        }
    }
}

void Parser::parseName()
{
    skipWhitespace();

    if (!isalpha(code[pos]) && code[pos] != '_')
    {
        Utils::parseError("Expected letter or \"_\", received \"" + std::string(1, code[pos]) + "\".", path, line, character);
    }

    int startLine = line;
    int startCharacter = character;

    std::string name;

    while (pos < code.size() && (isalnum(code[pos]) || code[pos] == '-' || code[pos] == '_'))
    {
        name += code[pos];

        nextCharacter();
    }

    if (name == "sequence-forwards")
    {
        tokens.push(new GroupOrder(startLine, startCharacter, Sequence::OrderEnum::Forwards));
    }

    else if (name == "sequence-backwards")
    {
        tokens.push(new GroupOrder(startLine, startCharacter, Sequence::OrderEnum::Backwards));
    }

    else if (name == "sequence-ping-pong")
    {
        tokens.push(new GroupOrder(startLine, startCharacter, Sequence::OrderEnum::PingPong));
    }

    else if (name == "sequence-random")
    {
        tokens.push(new GroupOrder(startLine, startCharacter, Sequence::OrderEnum::Random));
    }

    else if (name == "random-step")
    {
        tokens.push(new RandomType(startLine, startCharacter, Random::TypeEnum::Step));
    }

    else if (name == "random-linear")
    {
        tokens.push(new RandomType(startLine, startCharacter, Random::TypeEnum::Linear));
    }

    else
    {
        double base = 0;

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
                return tokens.push(new Name(startLine, startCharacter, name));
        }

        if (name.size() == 2 && isdigit(name[1]))
        {
            return tokens.push(new Constant(startLine, startCharacter, getFrequency(base + 12 * (name[1] - 48))));
        }

        if (name.size() == 3 && isdigit(name[2]))
        {
            if (name[1] == 's')
            {
                return tokens.push(new Constant(startLine, startCharacter, getFrequency(base + 12 * (name[2] - 48) + 1)));
            }

            if (name[1] == 'f')
            {
                return tokens.push(new Constant(startLine, startCharacter, getFrequency(base + 12 * (name[2] - 48) - 1)));
            }
        }

        tokens.push(new Name(startLine, startCharacter, name));
    }
}

void Parser::parseConstant()
{
    skipWhitespace();

    if (!isdigit(code[pos]) && code[pos] != '-')
    {
        Utils::parseError("Expected number, received \"" + std::string(1, code[pos]) + "\".", path, line, character);
    }

    int startLine = line;
    int startCharacter = character;

    std::string constant;

    constant += code[pos];

    nextCharacter();

    while (pos < code.size() && (isdigit(code[pos]) || code[pos] == '.'))
    {
        constant += code[pos];

        nextCharacter();
    }

    tokens.push(new Constant(startLine, startCharacter, std::stod(constant)));
}

void Parser::parseSingleChar(char c)
{
    skipWhitespace();

    if (code[pos] != c)
    {
        Utils::parseError("Expected \"" + std::string(1, c) + "\", received \"" + std::string(1, code[pos]) + "\".", path, line, character);
    }

    nextCharacter();
}

double Parser::getFrequency(double note)
{
    return 440 * pow(2, (note - 45) / 12);
}
