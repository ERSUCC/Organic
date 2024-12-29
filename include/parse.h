#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "location.h"
#include "token.h"

namespace Parser
{
    struct Parser
    {
        Parser(const std::string path);

        const Program* parse();

    private:
        void skipWhitespace();
        void nextCharacter();

        void tokenize();

        const BasicToken* getToken(const unsigned int pos) const;
        template <typename T> const T* getToken(const unsigned int pos) const;
        template <typename T> bool tokenIs(const unsigned int pos) const;

        void tokenError(const BasicToken* token, const std::string message) const;

        const Token* parseInstruction(unsigned int pos) const;
        const Define* parseDefine(unsigned int pos) const;
        const Assign* parseAssign(unsigned int pos) const;
        const Token* parseCall(unsigned int pos, const bool topLevel) const;
        const Argument* parseArgument(unsigned int pos) const;
        const Token* parseExpression(unsigned int pos) const;
        const List* parseList(unsigned int pos) const;
        const Token* parseTerms(unsigned int pos) const;
        const Token* parseTerm(unsigned int pos) const;

        double getFrequency(const double note) const;

        const std::string path;

        std::string code;

        std::vector<BasicToken*> tokens;

        unsigned int current = 0;
        unsigned int line = 1;
        unsigned int character = 1;

    };
}
