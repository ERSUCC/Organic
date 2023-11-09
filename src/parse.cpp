#include "../include/parse.h"

Parser::Parser(char* path)
{
    std::getline(std::ifstream(path), program, std::string::traits_type::to_char_type(std::string::traits_type::eof()));
}

Program* Parser::parse()
{
    Program* p = new Program();

    while (pos < program.size())
    {
        parseInstruction();

        p->instructions.push_back(getToken<Instruction>());

        skipWhitespace();
    }

    return p;
}

Token* Parser::getToken()
{
    lastToken = tokens.top();

    tokens.pop();

    return lastToken;
}

template <typename T> T* Parser::getToken()
{
    return dynamic_cast<T*>(getToken());
}

void Parser::skipWhitespace()
{
    while (isspace(program[pos]))
    {
        pos++;
    }
}

void Parser::parseInstruction()
{
    skipWhitespace();
    parseName();
    skipWhitespace();

    if (program[pos] == '(')
    {
        parseCall();
    }

    else
    {
        parseAssign();
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

    if (isalpha(program[pos]))
    {
        parseName();
        skipWhitespace();

        if (program[pos] == '(')
        {
            parseCall();
        }

        else
        {
            tokens.push(new Variable(getToken<Name>()->name));
        }
    }

    else if (program[pos] == '(')
    {
        parseList();
    }

    else
    {
        parseConstant();
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

        while (program[pos] != ')')
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
                List* list = dynamic_cast<List*>(argument->value);

                if (!list)
                {
                    Utils::error("Expected list.");
                }

                effects = list;
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

        while (program[pos] != ')')
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

        while (program[pos] != ')')
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

    else if (name->name == "delay")
    {
        Token* mix = new Constant(1);
        Token* delay = new Constant(0);
        Token* feedback = new Constant(0);

        while (program[pos] != ')')
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
    parseSingleChar('(');
    skipWhitespace();

    List* list = new List();

    while (program[pos] != ')')
    {
        parseExpression();

        list->items.push_back(getToken());

        skipWhitespace();

        if (program[pos] != ')')
        {
            parseSingleChar(',');
            skipWhitespace();
        }
    }

    tokens.push(list);

    parseSingleChar(')');
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

    if (program[pos] != ')')
    {
        parseSingleChar(',');
    }
}

void Parser::parseName()
{
    skipWhitespace();

    if (!isalpha(program[pos]))
    {
        Utils::error("Expected letter, received '" + std::string(1, program[pos]) + "'.");
    }

    std::string name;

    while (pos < program.size() && (isalnum(program[pos]) || program[pos] == '-' || program[pos] == '_'))
    {
        name += program[pos++];
    }

    tokens.push(new Name(name));
}

void Parser::parseConstant()
{
    skipWhitespace();

    if (!isdigit(program[pos]) && program[pos] != '-')
    {
        Utils::error("Expected number, received '" + std::string(1, program[pos]) + "'.");
    }

    std::string constant;

    while (pos < program.size() && (isdigit(program[pos]) || program[pos] == '.' || program[pos] == '-'))
    {
        constant += program[pos++];
    }

    tokens.push(new Constant(std::stod(constant)));
}

void Parser::parseSingleChar(char c)
{
    skipWhitespace();

    if (program[pos] != c)
    {
        Utils::error("Expected '" + std::string(1, c) + "', received '" + std::string(1, program[pos]) + "'.");
    }

    pos++;
}