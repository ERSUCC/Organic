#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <filesystem>

#include "token.h"
#include "object.h"

struct Parser
{
    Parser(const std::string path);

    Program* parse();

private:
    Token* getToken();
    template <typename T> T* getToken();

    void skipWhitespace();
    void nextCharacter();

    void parseInstruction();
    void parseComment();
    void parseAssign();
    void parseExpression();
    void parseCall();
    void parseList();
    void parseArgument();
    void parseName();
    void parseConstant();
    void parseSingleChar(char c);

    double getFrequency(double note);

    std::string sourcePath;
    std::string code;

    Program* program;

    std::stack<Token*> tokens;

    int pos = 0;
    int line = 1;
    int character = 1;

};
