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
    template <typename T> bool nextTokenIs();

    void skipWhitespace();

    void parseInstruction();
    void parseCall();
    bool parseArgument();
    void parseArgumentValue();
    void parseName();
    void parseConstant();
    void parseSingleChar(char c);

    std::string program;

    std::stack<Token*> tokens;

    Token* lastToken;

    int pos = 0;

};