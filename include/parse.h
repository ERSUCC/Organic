#pragma once

#include <algorithm>
#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "exception.h"
#include "location.h"
#include "path.h"
#include "source.h"
#include "token.h"
#include "tokenize.h"
#include "utils.h"

namespace Parser {

struct ParserContext
{
    ParserContext(ParserContext* parent, const std::string name, const std::vector<const InputDef*>& inputs);

    const VariableDef* addVariable(const Identifier* token, const Token* value);
    const FunctionDef* addFunction(const Identifier* token, const std::vector<const InputDef*>& inputs, const std::vector<const Token*>& instructions);

    const Identifier* findIdentifier(const Identifier* token);
    const FunctionRef* findFunction(const Identifier* token);

    void checkUsage() const;
    void merge(const ParserContext* context);

    ParserContext* parent;

private:
    void checkNameConflicts(const Identifier* token) const;
    bool checkRecursive(const Identifier* token) const;

    const std::string name;

    std::unordered_map<std::string, const InputDef*> inputs;
    std::unordered_map<std::string, const VariableDef*> variables;
    std::unordered_map<std::string, const FunctionDef*> functions;

    std::unordered_set<const Identifier*> used;

};

struct Parser
{
    static const Program* parseSource(const Path* path);
    static const Program* parseSource(const std::string& source);

private:
    Parser(const SourceProvider* source, ParserContext* context, std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths);

    const Program* parse();

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

    const SourceProvider* source;

    ParserContext* context;

    std::unordered_set<const Path*, Path::Hash, Path::Equals>& includedPaths;

    TokenList* tokens;

};

}
