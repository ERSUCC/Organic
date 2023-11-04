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
    template <typename T> T* getToken();

    void skipWhitespace();

    void parseInstruction();
    void parseName();
    void parseOpenParenthesis();
    void parseCloseParenthesis();

    std::string program;

    std::stack<Token*> tokens;

    Token* lastToken;

    int pos = 0;

};