#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "token.h"

namespace Parser
{
    struct Parser
    {
        Parser(const std::string path);

        Program* parse();

    private:
        void skipWhitespace();
        void nextCharacter();

        void tokenize();

        const Token* getToken(const unsigned int pos) const;
        template <typename T> const T* getToken(const unsigned int pos) const;
        template <typename T> bool tokenIs(const unsigned int pos) const;

        void tokenError(const Token* token, const std::string message) const;

        const Token* parseInstruction(unsigned int pos) const;
        const Token* parseDefine(unsigned int pos) const;
        const Token* parseAssign(unsigned int pos) const;
        const Token* parseCodeBlock(unsigned int pos) const;
        const Token* parseCall(unsigned int pos) const;
        const Token* parseArgument(unsigned int pos) const;
        const Token* parseExpression(unsigned int pos) const;
        const Token* parseTerms(unsigned int pos) const;
        const Token* parseTerm(unsigned int pos) const;

        double getFrequency(const double note) const;

        const std::string path;

        std::string code;

        std::vector<Token*> tokens;

        unsigned int current = 0;
        unsigned int line = 1;
        unsigned int character = 1;

    };
}
