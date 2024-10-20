#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "bytecode.h"
#include "controller.h"
#include "location.h"

namespace Parser
{
    struct BytecodeTransformer;

    enum class BasicReturnType
    {
        None,
        Any,
        SequenceOrder,
        RandomType,
        Value,
        Boolean,
        List,
        AudioSource,
        Effect
    };

    struct ReturnType
    {
        ReturnType(const BasicReturnType primaryType, ReturnType* subType = nullptr);

        std::string getTypeName() const;

        bool checkType(const ReturnType* other) const;

        ReturnType* subType;

    private:
        const BasicReturnType primaryType;

    };

    struct Token
    {
        Token(const SourceLocation location);

        virtual ReturnType* returnType(const BytecodeTransformer* visitor) const;

        virtual void accept(BytecodeTransformer* visitor) const;

        const SourceLocation location;
    };

    struct BasicToken : public Token
    {
        BasicToken(const SourceLocation location, const std::string str);

        const std::string str;
    };

    struct OpenParenthesis : public BasicToken
    {
        OpenParenthesis(const SourceLocation location);
    };

    struct CloseParenthesis : public BasicToken
    {
        CloseParenthesis(const SourceLocation location);
    };

    struct OpenSquareBracket : public BasicToken
    {
        OpenSquareBracket(const SourceLocation location);
    };

    struct CloseSquareBracket : public BasicToken
    {
        CloseSquareBracket(const SourceLocation location);
    };

    struct OpenCurlyBracket : public BasicToken
    {
        OpenCurlyBracket(const SourceLocation location);
    };

    struct CloseCurlyBracket : public BasicToken
    {
        CloseCurlyBracket(const SourceLocation location);
    };

    struct Colon : public BasicToken
    {
        Colon(const SourceLocation location);
    };

    struct Comma : public BasicToken
    {
        Comma(const SourceLocation location);
    };

    struct Equals : public BasicToken
    {
        Equals(const SourceLocation location);
    };

    struct Operator : public BasicToken
    {
        Operator(const SourceLocation location, const std::string str);
    };

    struct Add : public Operator
    {
        Add(const SourceLocation location);
    };

    struct Subtract : public Operator
    {
        Subtract(const SourceLocation location);
    };

    struct Multiply : public Operator
    {
        Multiply(const SourceLocation location);
    };

    struct Divide : public Operator
    {
        Divide(const SourceLocation location);
    };

    struct Power : public Operator
    {
        Power(const SourceLocation location);
    };

    struct DoubleEquals : public Operator
    {
        DoubleEquals(const SourceLocation location);
    };

    struct Less : public Operator
    {
        Less(const SourceLocation location);
    };

    struct Greater : public Operator
    {
        Greater(const SourceLocation location);
    };

    struct LessEqual : public Operator
    {
        LessEqual(const SourceLocation location);
    };

    struct GreaterEqual : public Operator
    {
        GreaterEqual(const SourceLocation location);
    };

    struct String : public BasicToken
    {
        String(const SourceLocation location, const std::string str);
    };

    struct Value : public BasicToken
    {
        Value(const SourceLocation location, const std::string str, const double value);

        ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const double value;
    };

    struct NamedConstant : public BasicToken
    {
        NamedConstant(const SourceLocation location, const std::string constant);

        ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct Variable : public BasicToken
    {
        Variable(const SourceLocation location, const std::string variable);

        ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct Argument : public Token
    {
        Argument(const SourceLocation location, const std::string name, const Token* value);

        const std::string name;

        const Token* value;
    };

    struct ArgumentList
    {
        ArgumentList(const std::vector<const Argument*> arguments, const std::string name);

        static ArgumentList* constructAlias(const std::vector<const Token*>& arguments, const std::string name);

        void get(const std::string name, BytecodeTransformer* visitor, ReturnType* expectedType = new ReturnType(BasicReturnType::Any));

        void confirmEmpty() const;

        unsigned char count = 0;

    private:
        std::vector<const Argument*> arguments;

        const std::string name;

    };

    struct List : public Token
    {
        List(const SourceLocation location, const std::vector<const Token*> values);

        ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const std::vector<const Token*> values;
    };

    struct ParenthesizedExpression : public Token
    {
        ParenthesizedExpression(const SourceLocation, const Token* value);

        ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value;
    };

    struct Assign : public Token
    {
        Assign(const SourceLocation location, const std::string variable, const Token* value);

        void accept(BytecodeTransformer* visitor) const override;

        const std::string variable;

        const Token* value;
    };

    struct Call : public Token
    {
        Call(const SourceLocation location, const std::string name, ArgumentList* arguments);

        ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const std::string name;

        ArgumentList* arguments;
    };

    struct CallAlias : public Call
    {
        CallAlias(const SourceLocation location, const std::string name, const std::vector<const Token*>& arguments);

        ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct Define : public Token
    {
        Define(const SourceLocation location, const std::string name, const std::vector<std::string> inputs, const std::vector<const Token*> instructions);

        ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const std::string name;

        const std::vector<std::string> inputs;

        const std::vector<const Token*> instructions;
    };

    struct Scope;

    struct VariableInfo
    {
        VariableInfo(const unsigned char id, const Token* value);

        const unsigned char id;

        const Token* value;
    };

    struct FunctionInfo
    {
        FunctionInfo(Scope* scope, ReturnType* returnType);

        Scope* scope;

        ReturnType* returnType;
    };

    struct InputInfo
    {
        InputInfo(const unsigned char id, ReturnType* returnType);

        const unsigned char id;

        ReturnType* returnType;
    };

    struct Scope
    {
        Scope(const BytecodeTransformer* visitor, Scope* parent = nullptr, const std::string currentFunction = "", const std::vector<std::string> inputs = std::vector<std::string>());

        VariableInfo* getVariable(const std::string name);
        VariableInfo* addVariable(const std::string name, const Token* value);

        FunctionInfo* getFunction(const std::string name);
        FunctionInfo* addFunction(const std::string name, Scope* scope, ReturnType* returnType);

        InputInfo* getInput(const std::string name);
        InputInfo* setInputType(const std::string name, ReturnType* returnType);

        bool checkRecursive(const std::string function) const;

        void checkUses() const;

        Scope* parent;

        const std::string currentFunction;

        BytecodeBlock* block;

    private:
        const BytecodeTransformer* visitor;

        std::unordered_map<std::string, VariableInfo*> variables;
        std::unordered_set<std::string> variablesUsed;

        std::unordered_map<std::string, FunctionInfo*> functions;
        std::unordered_set<std::string> functionsUsed;

        std::unordered_map<std::string, InputInfo*> inputs;
        std::unordered_set<std::string> inputsUsed;

    };

    struct Program
    {
        Program(const std::vector<const Token*> instructions);

        const std::vector<const Token*> instructions;
    };

    struct BytecodeTransformer
    {
        BytecodeTransformer(const std::string sourcePath);

        std::string transform(const Program* program);

        void visit(const Value* token);
        void visit(const NamedConstant* token);
        void visit(const Variable* token);
        void visit(const List* token);
        void visit(const ParenthesizedExpression* token);
        void visit(const Assign* token);
        void visit(const Call* token);
        void visit(const CallAlias* token);
        void visit(const Define* token);
        void visit(const Program* token);

        Scope* currentScope;

        ReturnType* expectedType = new ReturnType(BasicReturnType::Any);

    private:
        std::string outputPath;

        BytecodeResolver* resolver = new BytecodeResolver();

        Utils* utils;

    };
}
