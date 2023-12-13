#include "../include/parse.h"

Parser::Parser(const char* path)
{
    std::ifstream file(path);
    
    if (!file.is_open())
    {
        Utils::error("Could not open the specified organic program.");
    }

    std::getline(file, code, std::string::traits_type::to_char_type(std::string::traits_type::eof()));
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

void Parser::skipWhitespace()
{
    while (pos < code.size() && isspace(code[pos]))
    {
        pos++;
    }
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
        pos++;
    }
}

void Parser::parseAssign()
{
    Name* name = getToken<Name>();

    skipWhitespace();
    parseSingleChar('=');
    parseExpression();

    tokens.push(new Assign(name->name, getToken()));
}

void Parser::parseExpression()
{
    skipWhitespace();

    if (isalpha(code[pos]))
    {
        parseName();
        skipWhitespace();

        if (code[pos] == '(')
        {
            parseCall();
        }

        else
        {
            Token* token = getToken();

            if (dynamic_cast<Name*>(token))
            {
                tokens.push(new VariableRef(dynamic_cast<Name*>(token)->name));
            }

            else
            {
                tokens.push(token);
            }

            if (code[pos] == '+')
            {
                Token* value1 = getToken();

                parseSingleChar('+');
                skipWhitespace();
                parseExpression();

                tokens.push(new CreateValueAdd(value1, getToken()));
            }

            else if (code[pos] == '-')
            {
                Token* value1 = getToken();

                parseSingleChar('-');
                skipWhitespace();
                parseExpression();

                tokens.push(new CreateValueSubtract(value1, getToken()));
            }
        }
    }

    else if (code[pos] == '[')
    {
        parseList();
    }

    else if (code[pos] == '(')
    {
        parseSingleChar('(');
        parseExpression();
        parseSingleChar(')');
    }

    else
    {
        parseConstant();
    }

    skipWhitespace();

    if (code[pos] == '+')
    {
        Token* value1 = getToken();

        parseSingleChar('+');
        skipWhitespace();
        parseExpression();

        tokens.push(new CreateValueAdd(value1, getToken()));
    }

    else if (code[pos] == '-')
    {
        Token* value1 = getToken();

        parseSingleChar('-');
        skipWhitespace();
        parseExpression();

        tokens.push(new CreateValueSubtract(value1, getToken()));
    }
}

void Parser::parseCall()
{
    skipWhitespace();
    parseSingleChar('(');

    Name* name = getToken<Name>();

    if (name->name == "sine" || name->name == "square" || name->name == "saw" || name->name == "triangle")
    {
        Token* volume = new Constant(1);
        Token* pan = new Constant(0);
        Token* frequency = new Constant(0);
        List* effects = new List();

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
                Utils::error("Unknown argument name '" + argument->name->name + "'.");
            }

            skipWhitespace();
        }

        if (name->name == "sine")
        {
            tokens.push(new CreateSine(volume, pan, frequency, effects));
        }
        
        else if (name->name == "square")
        {
            tokens.push(new CreateSquare(volume, pan, frequency, effects));
        }

        else if (name->name == "saw")
        {
            tokens.push(new CreateSaw(volume, pan, frequency, effects));
        }

        else if (name->name == "triangle")
        {
            tokens.push(new CreateTriangle(volume, pan, frequency, effects));
        }
    }

    else if (name->name == "hold")
    {
        Token* value = new Constant(0);
        Token* length = new Constant(0);

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
                Utils::error("Unknown argument name '" + argument->name->name + "'.");
            }

            skipWhitespace();
        }

        tokens.push(new CreateHold(value, length));
    }

    else if (name->name == "lfo" || name->name == "sweep")
    {
        Token* repeats = new Constant(0);
        Token* from = new Constant(0);
        Token* to = new Constant(1);
        Token* length = new Constant(0);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "repeats")
            {
                repeats = argument->value;
            }

            else if (argument->name->name == "from")
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
                Utils::error("Unknown argument name '" + argument->name->name + "'.");
            }

            skipWhitespace();
        }

        if (name->name == "lfo")
        {
            tokens.push(new CreateLFO(repeats, from, to, length));
        }

        else if (name->name == "sweep")
        {
            tokens.push(new CreateSweep(repeats, from, to, length));
        }
    }

    else if (name->name == "sequence")
    {
        Token* repeats = new Constant(0);
        List* values = new List();
        Token* order = new GroupOrder(ControllerGroup::OrderEnum::Forwards);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "repeats")
            {
                repeats = argument->value;
            }

            else if (argument->name->name == "values")
            {
                values = (List*)argument->value;
            }

            else if (argument->name->name == "order")
            {
                order = argument->value;
            }

            else
            {
                Utils::error("Unknown argument name '" + argument->name->name + "'.");
            }

            skipWhitespace();
        }

        tokens.push(new CreateControllerGroup(repeats, values, order));
    }

    else if (name->name == "random")
    {
        Token* repeats = new Constant(0);
        Token* from = new Constant(0);
        Token* to = new Constant(1);
        Token* length = new Constant(0);
        RandomType* type = new RandomType(Random::TypeEnum::Step);

        while (code[pos] != ')')
        {
            parseArgument();

            Argument* argument = getToken<Argument>();

            if (argument->name->name == "repeats")
            {
                repeats = argument->value;
            }

            else if (argument->name->name == "from")
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
                Utils::error("Unknown argument name '" + argument->name->name + "'.");
            }

            skipWhitespace();
        }

        tokens.push(new CreateRandom(repeats, from, to, length, type));
    }

    else if (name->name == "delay")
    {
        Token* mix = new Constant(1);
        Token* delay = new Constant(0);
        Token* feedback = new Constant(0);

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
                Utils::error("Unknown argument name '" + argument->name->name + "'.");
            }

            skipWhitespace();
        }

        tokens.push(new CreateDelay(mix, delay, feedback));
    }

    else
    {
        Utils::error("Unknown function '" + name->name + "'.");
    }

    parseSingleChar(')');
}

void Parser::parseList()
{
    skipWhitespace();
    parseSingleChar('[');
    skipWhitespace();

    List* list = new List();

    while (code[pos] != ']')
    {
        parseExpression();

        list->items.push_back(getToken());

        skipWhitespace();

        if (code[pos] != ']')
        {
            parseSingleChar(',');
            skipWhitespace();
        }
    }

    tokens.push(list);

    parseSingleChar(']');
}

void Parser::parseArgument()
{
    skipWhitespace();
    parseName();

    Name* name = getToken<Name>();

    parseSingleChar(':');
    parseExpression();

    tokens.push(new Argument(name, getToken()));

    skipWhitespace();

    if (code[pos] != ')')
    {
        parseSingleChar(',');
    }
}

void Parser::parseName()
{
    skipWhitespace();

    if (!isalpha(code[pos]) && code[pos] != '_')
    {
        Utils::error("Expected letter or '_', received '" + std::string(1, code[pos]) + "'.");
    }

    std::string name;

    while (pos < code.size() && (isalnum(code[pos]) || code[pos] == '-' || code[pos] == '_'))
    {
        name += code[pos++];
    }

    if (name == "sequence-forwards")
    {
        tokens.push(new GroupOrder(ControllerGroup::OrderEnum::Forwards));
    }

    else if (name == "sequence-backwards")
    {
        tokens.push(new GroupOrder(ControllerGroup::OrderEnum::Backwards));
    }

    else if (name == "sequence-ping-pong")
    {
        tokens.push(new GroupOrder(ControllerGroup::OrderEnum::PingPong));
    }

    else if (name == "sequence-random")
    {
        tokens.push(new GroupOrder(ControllerGroup::OrderEnum::Random));
    }

    else if (name == "random-step")
    {
        tokens.push(new RandomType(Random::TypeEnum::Step));
    }

    else
    {
        tokens.push(new Name(name));
    }
}

void Parser::parseConstant()
{
    skipWhitespace();

    if (!isdigit(code[pos]) && code[pos] != '-')
    {
        Utils::error("Expected number, received '" + std::string(1, code[pos]) + "'.");
    }

    std::string constant;

    while (pos < code.size() && (isdigit(code[pos]) || code[pos] == '.' || code[pos] == '-'))
    {
        constant += code[pos++];
    }

    tokens.push(new Constant(std::stod(constant)));
}

void Parser::parseSingleChar(char c)
{
    skipWhitespace();

    if (code[pos] != c)
    {
        Utils::error("Expected '" + std::string(1, c) + "', received '" + std::string(1, code[pos]) + "'.");
    }

    pos++;
}