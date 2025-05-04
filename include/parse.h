#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "location.h"
#include "path.h"
#include "token.h"
#include "tokenize.h"
#include "utils.h"

namespace Parser
{
    struct ParserContext
    {
        ParserContext(ParserContext* parent, const std::string name, const std::vector<InputDef*>& inputs);

        VariableDef* addVariable(const Identifier* token);
        FunctionDef* addFunction(const Identifier* token, const std::vector<InputDef*>& inputs);

        Identifier* findIdentifier(const Identifier* token);
        FunctionRef* findFunction(const Identifier* token);

        void merge(const ParserContext* context);

        ParserContext* parent;

    private:
        void checkNameConflicts(const Identifier* token) const;
        bool checkRecursive(const Identifier* token) const;

        const std::string name;

        std::unordered_map<std::string, InputDef*> inputs;
        std::unordered_map<std::string, VariableDef*> variables;
        std::unordered_map<std::string, FunctionDef*> functions;

    };

    struct Parser
    {
        Parser(const Path* path, ParserContext* context, std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths);

        Program* parse();

    private:
        void tokenError(const Token* token, const std::string message) const;

        TokenListNode* parseInclude(TokenListNode* start);
        TokenListNode* parseInstruction(TokenListNode* start);
        TokenListNode* parseDefine(TokenListNode* start);
        TokenListNode* parseAssign(TokenListNode* start);
        TokenListNode* parseCall(TokenListNode* start);
        TokenListNode* parseArgument(TokenListNode* start);
        TokenListNode* parseExpression(TokenListNode* start);
        TokenListNode* parseList(TokenListNode* start);
        TokenListNode* parseTerms(TokenListNode* start);
        TokenListNode* parseTerm(TokenListNode* start);

        const Path* path;

        ParserContext* context;

        std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths;

        TokenList* tokens;

    };
}
