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

    Token* getToken(const unsigned int pos) const;
    template <typename T> T* getToken(const unsigned int pos) const;
    template <typename T> bool tokenIs(const unsigned int pos) const;

    void tokenError(const Token* token, const std::string message) const;

    TokenRange* parseInstruction(unsigned int pos) const;
    TokenRange* parseAssign(unsigned int pos) const;
    TokenRange* parseCodeBlock(unsigned int pos) const;
    TokenRange* parseCall(unsigned int pos) const;
    TokenRange* parseArgument(unsigned int pos) const;
    TokenRange* parseExpression(unsigned int pos) const;
    TokenRange* parseTerms(unsigned int pos) const;
    TokenRange* parseTerm(unsigned int pos) const;

    const std::string path;

    std::string code;

    std::vector<Token*> tokens;

    unsigned int current = 0;
    unsigned int line = 1;
    unsigned int character = 1;

};
