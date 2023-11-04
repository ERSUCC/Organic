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

        Instruction* instruction = getToken<Instruction>();

        if (!instruction)
        {
            Organic::error("Expected instruction, received '" + lastToken->toString() + "'.");
        }

        p->instructions.push_back(instruction);
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

template <typename T> bool Parser::nextTokenIs()
{
    return dynamic_cast<T*>(tokens.top());
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
    parseCall();
}

void Parser::parseCall()
{
    skipWhitespace();
    parseName();
    parseSingleChar('(');

    Name* name = getToken<Name>();

    if (name->name == "sine")
    {
        Token* volume = new Constant(1);
        Token* pan = new Constant(0);
        Token* frequency = new Constant(0);

        while (parseArgument())
        {
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

            else
            {
                Organic::error("Unknown argument name '" + argument->name->name + "'.");
            }
        }

        tokens.push(new CreateSine(volume, pan, frequency));
    }

    else if (name->name == "lfo")
    {
        Token* repeats = new Constant(0);
        Token* floor = new Constant(0);
        Token* ceiling = new Constant(1);
        Token* rate = new Constant(1);

        while (parseArgument())
        {
            Argument* argument = getToken<Argument>();

            if (argument->name->name == "repeats")
            {
                repeats = argument->value;
            }

            else if (argument->name->name == "floor")
            {
                floor = argument->value;
            }

            else if (argument->name->name == "ceiling")
            {
                ceiling = argument->value;
            }

            else if (argument->name->name == "rate")
            {
                rate = argument->value;
            }

            else
            {
                Organic::error("Unknown argument name '" + argument->name->name + "'.");
            }
        }

        tokens.push(new CreateLFO(repeats, floor, ceiling, rate));
    }

    else
    {
        Organic::error("Unknown function '" + name->name + "'.");
    }

    parseSingleChar(')');
}

bool Parser::parseArgument()
{
    skipWhitespace();

    if (program[pos] == ')')
    {
        return false;
    }

    parseName();

    Name* name = getToken<Name>();

    parseSingleChar(':');
    parseArgumentValue();

    tokens.push(new Argument(name, getToken()));

    skipWhitespace();

    if (program[pos] == ',')
    {
        pos++;
    }

    return true;
}

void Parser::parseArgumentValue()
{
    skipWhitespace();

    if (isdigit(program[pos]))
    {
        parseConstant();
    }

    else
    {
        parseCall();
    }
}

void Parser::parseName()
{
    skipWhitespace();

    if (!isalpha(program[pos]))
    {
        Organic::error("Expected letter, received '" + std::string(1, program[pos]) + "'.");
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

    if (!isdigit(program[pos]))
    {
        Organic::error("Expected number, received '" + std::string(1, program[pos]) + "'.");
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
        Organic::error("Expected '" + std::string(1, c) + "', received '" + std::string(1, program[pos]) + "'.");
    }

    pos++;
}