#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "location.h"
#include "path.h"
#include "token.h"
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
        Parser(const Path* path, ParserContext* context);

        Program* parse();

    private:
        void skipWhitespace();
        void nextCharacter();

        void tokenize();

        BasicToken* getToken(const unsigned int pos) const;
        template <typename T> T* getToken(const unsigned int pos) const;
        template <typename T> bool tokenIs(const unsigned int pos) const;

        void tokenError(const BasicToken* token, const std::string message) const;

        Token* parseInstruction(unsigned int pos);
        Define* parseDefine(unsigned int pos);
        Assign* parseAssign(unsigned int pos) const;
        Token* parseCall(unsigned int pos, const bool top) const;
        Argument* parseArgument(unsigned int pos) const;
        Token* parseExpression(unsigned int pos) const;
        List* parseList(unsigned int pos) const;
        Token* parseTerms(unsigned int pos) const;
        Token* parseTerm(unsigned int pos) const;

        double getFrequency(const double note) const;

        const Path* path;

        ParserContext* context;

        std::string code;

        std::vector<BasicToken*> tokens;

        unsigned int current = 0;
        unsigned int line = 1;
        unsigned int character = 1;

    };

    struct ParserCreator : public ParserInterface
    {
        Program* parse(const Path* path) override;
    };
}
