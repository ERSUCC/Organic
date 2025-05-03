#pragma once

#include <string>
#include <vector>

#include "location.h"
#include "path.h"
#include "token.h"
#include "utils.h"

namespace Parser
{
    struct Tokenizer
    {
        Tokenizer(const Path* path);

        std::vector<BasicToken*> tokenize();

    private:
        void tokenizeString(const unsigned int line, const unsigned int character);
        void tokenizeNumber(const unsigned int line, const unsigned int character);
        void tokenizeIdentifier(const unsigned int line, const unsigned int character);

        void skipWhitespace();
        void nextCharacter();

        double getFrequency(const double note) const;

        BasicToken* getToken(const unsigned int pos) const;
        template <typename T> bool tokenIs(const unsigned int pos) const;

        const Path* path;

        Utils* utils;

        std::string code;

        std::vector<BasicToken*> tokens;

        unsigned int current = 0;
        unsigned int line = 1;
        unsigned int character = 1;

    };
}
