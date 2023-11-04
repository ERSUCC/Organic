#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

#include "organic.h"
#include "token.h"

struct Parser
{
    Parser(char* path);

    Program* parse();

private:
    Token* getToken();
    template <typename T> T* getToken();

    void skipWhitespace();

    void parseInstruction();
    bool parseArgument();
    void parseName();
    void parseConstant();
    void parseOpenParenthesis();
    void parseCloseParenthesis();
    void parseEquals();
    void parseColon();

    std::string program;

    std::stack<Token*> tokens;

    Token* lastToken;

    int pos = 0;

};