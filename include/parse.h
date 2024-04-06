#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "token.h"

struct Parser
{
    Parser(const std::string path);

    Program* parse();

private:
    void skipWhitespace();
    void nextCharacter();

    void tokenize();

    Token* getToken(const int pos);
    template <typename T> T* getToken(const int pos);
    template <typename T> bool tokenIs(const int pos);

    void tokenError(const Token* token, const std::string message);

    TokenRange* parseInstruction(int pos);
    TokenRange* parseAssign(int pos);
    TokenRange* parseCodeBlock(int pos);
    TokenRange* parseCall(int pos);
    TokenRange* parseArgument(int pose);
    TokenRange* parseExpression(int pos);
    TokenRange* parseTerms(int pos);
    TokenRange* parseTerm(int pos);

    const std::string path;

    std::string code;

    std::vector<Token*> tokens;

    int current = 0;
    int line = 1;
    int character = 1;

};
