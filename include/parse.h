#pragma once

#include <algorithm>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "exception.h"
#include "location.h"
#include "path.h"
#include "source.h"
#include "token.h"
#include "token_decls.h"
#include "tokenize.h"
#include "utils.h"

namespace Parser {

struct ParserContext
{
    ParserContext(ParserContext* parent, const std::string name, const std::vector<UniqueToken<InputDef>>& inputs);
    ~ParserContext();

    const VariableDef* addVariable(const Identifier* token, const Token* value);
    const FunctionDef* addFunction(const Identifier* token, std::vector<UniqueToken<InputDef>>& inputs, const Program* program);

    const Identifier* findIdentifier(const Identifier* token);
    const FunctionDef* findFunction(const Identifier* token);

    void addInstruction(const Token* instruction);

    void checkNameConflicts(const Identifier* token) const;
    bool checkRecursive(const Identifier* token) const;
    void checkUsage() const;

    const Program* buildProgram(const SourceProvider* source);

    ParserContext* parent;

private:
    const std::string name;

    std::unordered_map<std::string, const InputDef*> inputs;
    std::unordered_map<std::string, const VariableDef*> variables;
    std::unordered_map<std::string, const FunctionDef*> functions;

    std::unordered_set<const Identifier*> used;

    std::vector<const Token*> instructions;

};

struct Parser
{
    static const Program* parseSource(const SourceProvider* source);

private:
    Parser(const SourceProvider* source, ParserContext* context, std::unordered_set<Path, Path::Hash, Path::Equals>& includedPaths);
    ~Parser();

    const void parseProgram();
    const void parseInclude();
    const void parseInstruction();
    const void parseAssign();
    const void parseDefine();

    const Token* parseExpression(const std::string& errorContext);
    const List* parseList();
    const Token* parseTerms(const std::string& errorContext);
    const Token* collapseTerms(const SourceLocation& location, std::vector<UniqueToken<>>& terms, const size_t start, const size_t end) const;

    UniqueToken<> parseTerm(const std::string& errorContext);
    UniqueToken<Call> parseCall();
    UniqueToken<Argument> parseArgument(const std::string& errorContext);

    const SourceProvider* source;

    ParserContext* context;

    std::unordered_set<Path, Path::Hash, Path::Equals>& includedPaths;

    TokenIterator* tokens;

};

}
