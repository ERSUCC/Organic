#pragma once

#include <string>
#include <vector>

#include "location.h"
#include "path.h"
#include "token.h"
#include "utils.h"

namespace Parser
{
    struct TokenListNode
    {
        TokenListNode(Token* token, TokenListNode* prev, TokenListNode* next, const bool end);

        template <typename T> T* getToken() const
        {
            return dynamic_cast<T*>(token);
        }

        Token* token;

        TokenListNode* prev;
        TokenListNode* next;

        const bool end;
    };

    struct TokenList
    {
        TokenList();

        void add(Token* token);

        TokenListNode* stitch(TokenListNode* start, TokenListNode* end);
        TokenListNode* patch(TokenListNode* start, TokenListNode* end, Token* token);

        TokenListNode* head = new TokenListNode(new Token(SourceLocation(nullptr, 0, 0)), nullptr, nullptr, true);
        TokenListNode* tail = new TokenListNode(new Token(SourceLocation(nullptr, 0, 0)), nullptr, nullptr, true);
    };

    struct Tokenizer
    {
        Tokenizer(const Path* path);

        TokenList* tokenize();

    private:
        Token* tokenizeString();
        Token* tokenizeNumber();
        Token* tokenizeIdentifier();

        void skipWhitespace();
        void nextCharacter();

        double getFrequency(const double note) const;

        const Path* path;

        Utils* utils;

        std::string code;

        unsigned int current = 0;
        unsigned int line = 1;
        unsigned int character = 1;

    };
}
