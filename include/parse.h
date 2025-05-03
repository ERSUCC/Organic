#pragma once

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
        BasicToken* getToken(const unsigned int pos) const;
        template <typename T> T* getToken(const unsigned int pos) const;
        template <typename T> bool tokenIs(const unsigned int pos) const;

        void tokenError(const BasicToken* token, const std::string message) const;

        Include* parseInclude(unsigned int pos) const;
        Token* parseInstruction(unsigned int pos);
        Define* parseDefine(unsigned int pos);
        Assign* parseAssign(unsigned int pos) const;
        Token* parseCall(unsigned int pos) const;
        Argument* parseArgument(unsigned int pos) const;
        Token* parseExpression(unsigned int pos) const;
        List* parseList(unsigned int pos) const;
        Token* parseTerms(unsigned int pos) const;
        Token* parseTerm(unsigned int pos) const;

        const Path* path;

        ParserContext* context;

        std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths;

        std::vector<BasicToken*> tokens;

    };
}
