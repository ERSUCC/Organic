#pragma once

#include <stddef.h>
#include <string>
#include <vector>

#include "constants.h"
#include "exception.h"
#include "location.h"
#include "source.h"
#include "token.h"
#include "utils.h"

namespace Parser {

struct OrganicTokenException : public OrganicParseException
{
    OrganicTokenException(const Token* token, const std::string& expected);

    const Token* token;

    const std::string expected;

private:
    static std::string getMessage(const Token* token, const std::string& expected);

};

struct TokenIterator
{
    ~TokenIterator();

    inline void push(const Token* token)
    {
        tokens.push_back(token);
    }

    const Token* peek(const size_t offset = 0) const;

    template <typename T> const T* peek(const size_t offset = 0) const
    {
        if (current + offset < tokens.size())
        {
            return dynamic_cast<const T*>(tokens[current + offset]);
        }

        return nullptr;
    }

    template <typename T> const T* require(const std::string& expected)
    {
        if (const T* token = dynamic_cast<const T*>(tokens[current]))
        {
            tokens[current++] = nullptr;

            return token;
        }

        throw OrganicTokenException(tokens[current], expected);
    }

    template <typename T> TokenIterator* expect(const std::string& expected)
    {
        if (!dynamic_cast<const T*>(tokens[current]))
        {
            throw OrganicTokenException(tokens[current], expected);
        }

        delete tokens[current];

        tokens[current++] = nullptr;

        return this;
    }

    template <typename T = Token> const T* take()
    {
        if (current < tokens.size() - 1)
        {
            const T* token = dynamic_cast<const T*>(tokens[current]);

            tokens[current++] = nullptr;

            return token;
        }

        return dynamic_cast<const T*>(tokens[current]);
    }

    TokenIterator* drop(const size_t count = 1);

    template <typename T> const T* last()
    {
        if (tokens.empty())
        {
            return nullptr;
        }

        return dynamic_cast<const T*>(tokens.back());
    }

private:
    std::vector<const Token*> tokens;

    size_t current = 0;

};

struct Tokenizer
{
    static TokenIterator* tokenize(const SourceProvider* source);

private:
    Tokenizer(const SourceProvider* source);

    TokenIterator* tokenizeProgram();

    const Token* tokenizeString();
    const Token* tokenizeNumber();
    const Token* tokenizeIdentifier();

    void skipWhitespace();

    double getFrequency(const double note) const;

    const SourceProvider* source;

    const Utils* utils;

    size_t current = 0;

};

}
