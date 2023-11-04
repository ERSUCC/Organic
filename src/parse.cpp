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

template <typename T> T* Parser::getToken()
{
    lastToken = tokens.top();

    tokens.pop();

    return dynamic_cast<T*>(lastToken);
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
    parseName();

    Name* name = getToken<Name>();

    if (!name)
    {
        Organic::error("Expected name, received '" + lastToken->toString() + "'.");
    }

    parseOpenParenthesis();
    parseCloseParenthesis();

    if (name->name == "sine")
    {
        tokens.push(new CreateSine(new Constant(1), new Constant(0), new Constant(200)));
    }

    else
    {
        Organic::error("Unknown keyword '" + name->name + "'.");
    }
}

void Parser::parseName()
{
    skipWhitespace();

    std::string name;

    if (!isalpha(program[pos]))
    {
        Organic::error("Expected letter, received '" + std::string(1, program[pos]) + "'.");
    }

    while (pos < program.size() && isalnum(program[pos]))
    {
        name += program[pos++];
    }

    tokens.push(new Name(name));
}

void Parser::parseOpenParenthesis()
{
    skipWhitespace();

    if (program[pos++] != '(')
    {
        Organic::error("Expected '(', received '" + std::string(1, program[pos]) + "'.");
    }
}

void Parser::parseCloseParenthesis()
{
    skipWhitespace();

    if (program[pos++] != ')')
    {
        Organic::error("Expected ')', received '" + std::string(1, program[pos]) + "'.");
    }
}