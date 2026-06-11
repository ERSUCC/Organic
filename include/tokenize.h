#pragma once

#include <string>

#include "exception.h"
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
        TokenList(const SourceFile* source);

        void add(Token* token);

        TokenListNode* stitch(TokenListNode* start, TokenListNode* end);
        TokenListNode* patch(TokenListNode* start, TokenListNode* end, Token* token);

        TokenListNode* head;
        TokenListNode* tail;
    };

    struct Tokenizer
    {
        Tokenizer(const SourceFile* source);

        TokenList* tokenize();

    private:
        Token* tokenizeString();
        Token* tokenizeNumber();
        Token* tokenizeIdentifier();

        void skipWhitespace();

        double getFrequency(const double note) const;

        const SourceFile* source;

        const Utils* utils;

        size_t current = 0;

    };
}
