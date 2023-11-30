#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

#include "token.h"
#include "object.h"

struct Parser
{
    Parser(char* path);

    Program* parse();

private:
    Token* getToken();
    template <typename T> T* getToken();

    void skipWhitespace();

    void parseInstruction();
    void parseAssign();
    void parseExpression();
    void parseCall();
    void parseList();
    void parseArgument();
    void parseName();
    void parseConstant();
    void parseSingleChar(char c);

    std::string program;

    std::stack<Token*> tokens;

    int pos = 0;

};