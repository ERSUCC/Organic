#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "bytecode.h"
#include "controller.h"

namespace Parser
{
    struct BytecodeTransformer;

    struct ParserLocation
    {
        ParserLocation(const unsigned int line, const unsigned int character, const unsigned int start, const unsigned int end);

        const unsigned int line;
        const unsigned int character;
        const unsigned int start;
        const unsigned int end;
    };

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
        ReturnType(const BasicReturnType primaryType, const ReturnType* subType = nullptr);

        std::string getTypeName() const;

        bool checkType(const ReturnType* other) const;

    private:
        const BasicReturnType primaryType;
        const ReturnType* subType;

    };

    struct Token
    {
        Token();
        Token(const ParserLocation location);

        virtual const ReturnType* returnType(const BytecodeTransformer* visitor) const;

        virtual void accept(BytecodeTransformer* visitor) const;

        const ParserLocation location = ParserLocation(0, 0, 0, 0);
    };

    struct BasicToken : public Token
    {
        BasicToken();
        BasicToken(const ParserLocation location, const std::string str);

        const std::string str;
    };

    struct OpenParenthesis : public BasicToken
    {
        OpenParenthesis(const ParserLocation location);
    };

    struct CloseParenthesis : public BasicToken
    {
        CloseParenthesis(const ParserLocation location);
    };

    struct OpenSquareBracket : public BasicToken
    {
        OpenSquareBracket(const ParserLocation location);
    };

    struct CloseSquareBracket : public BasicToken
    {
        CloseSquareBracket(const ParserLocation location);
    };

    struct OpenCurlyBracket : public BasicToken
    {
        OpenCurlyBracket(const ParserLocation location);
    };

    struct CloseCurlyBracket : public BasicToken
    {
        CloseCurlyBracket(const ParserLocation location);
    };

    struct Colon : public BasicToken
    {
        Colon(const ParserLocation location);
    };

    struct Comma : public BasicToken
    {
        Comma(const ParserLocation location);
    };

    struct Equals : public BasicToken
    {
        Equals(const ParserLocation location);
    };

    struct Operator : public BasicToken
    {
        Operator(const ParserLocation location, const std::string str);
    };

    struct Add : public Operator
    {
        Add(const ParserLocation location);
    };

    struct Subtract : public Operator
    {
        Subtract(const ParserLocation location);
    };

    struct Multiply : public Operator
    {
        Multiply(const ParserLocation location);
    };

    struct Divide : public Operator
    {
        Divide(const ParserLocation location);
    };

    struct Power : public Operator
    {
        Power(const ParserLocation location);
    };

    struct DoubleEquals : public Operator
    {
        DoubleEquals(const ParserLocation location);
    };

    struct Less : public Operator
    {
        Less(const ParserLocation location);
    };

    struct Greater : public Operator
    {
        Greater(const ParserLocation location);
    };

    struct LessEqual : public Operator
    {
        LessEqual(const ParserLocation location);
    };

    struct GreaterEqual : public Operator
    {
        GreaterEqual(const ParserLocation location);
    };

    struct String : public BasicToken
    {
        String(const ParserLocation location, const std::string str);
    };

    struct Value : public BasicToken
    {
        Value(const ParserLocation location, const std::string str, const double value);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const double value;
    };

    struct NamedConstant : public BasicToken
    {
        NamedConstant(const ParserLocation location, const std::string constant);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct Variable : public BasicToken
    {
        Variable(const ParserLocation location, const std::string variable);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct Argument : public Token
    {
        Argument(const ParserLocation location, const std::string name, const Token* value);

        const std::string name;

        const Token* value;
    };

    struct ArgumentList
    {
        ArgumentList(const std::vector<const Argument*> arguments, const std::string name, const std::string path);

        void get(const std::string name, BytecodeTransformer* visitor, ReturnType* expectedType = new ReturnType(BasicReturnType::Any));

        void confirmEmpty() const;

        unsigned char count = 0;

    private:
        std::vector<const Argument*> arguments;

        const std::string name;
        const std::string path;

    };

    struct List : public Token
    {
        List(const ParserLocation location, const std::vector<const Token*> values);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const std::vector<const Token*> values;
    };

    struct OperatorObject : public Token
    {
        OperatorObject(const ParserLocation location, const Token* value1, const Token* value2);

        const Token* value1;
        const Token* value2;
    };

    struct ArithmeticExpression : public OperatorObject
    {
        ArithmeticExpression(const ParserLocation location, const Token* value1, const Token* value2);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;
    };

    struct AddObject : public ArithmeticExpression
    {
        AddObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct SubtractObject : public ArithmeticExpression
    {
        SubtractObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct MultiplyObject : public ArithmeticExpression
    {
        MultiplyObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct DivideObject : public ArithmeticExpression
    {
        DivideObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct PowerObject : public ArithmeticExpression
    {
        PowerObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct ConditionalExpression : public OperatorObject
    {
        ConditionalExpression(const ParserLocation location, const Token* value1, const Token* value2);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;
    };

    struct EqualsObject : public ConditionalExpression
    {
        EqualsObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct LessObject : public ConditionalExpression
    {
        LessObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct GreaterObject : public ConditionalExpression
    {
        GreaterObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct LessEqualObject : public ConditionalExpression
    {
        LessEqualObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct GreaterEqualObject : public ConditionalExpression
    {
        GreaterEqualObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
        const Token* value2;
    };

    struct ParenthesizedExpression : public Token
    {
        ParenthesizedExpression(const ParserLocation, const Token* value);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value;
    };

    struct Assign : public Token
    {
        Assign(const ParserLocation location, const std::string variable, const Token* value);

        void accept(BytecodeTransformer* visitor) const override;

        const std::string variable;

        const Token* value;
    };

    struct Call : public Token
    {
        Call(const ParserLocation location, const std::string name, ArgumentList* arguments);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const std::string name;

        ArgumentList* arguments;
    };

    struct Define : public Token
    {
        Define(const ParserLocation location, const std::string name, const std::vector<std::string> inputs, const std::vector<const Token*> instructions);

        const ReturnType* returnType(const BytecodeTransformer* visitor) const override;

        void accept(BytecodeTransformer* visitor) const override;

        const std::string name;

        const std::vector<std::string> inputs;

        const std::vector<const Token*> instructions;
    };

    struct VariableInfo
    {
        VariableInfo(const unsigned char id, const Token* value);

        const unsigned char id;

        const Token* value;
    };

    struct FunctionInfo
    {
        FunctionInfo(const BytecodeBlock* body, const ReturnType* returnType);

        const BytecodeBlock* body;

        const ReturnType* returnType;
    };

    struct Scope
    {
        Scope(const BytecodeTransformer* visitor, Scope* parent = nullptr, const std::string currentFunction = "", const std::vector<std::string> inputs = std::vector<std::string>());

        VariableInfo* getVariable(const std::string name);
        VariableInfo* addVariable(const std::string name, const Token* value);

        FunctionInfo* getFunction(const std::string name);
        FunctionInfo* addFunction(const std::string name, const BytecodeBlock* body, const ReturnType* returnType);

        std::optional<unsigned char> getInput(const std::string input);

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

        std::unordered_map<std::string, unsigned char> inputs;
        std::unordered_set<std::string> inputsUsed;

    };

    struct Program : public Token
    {
        Program(const std::vector<const Token*> instructions);

        void accept(BytecodeTransformer* visitor) const override;

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
        void visit(const AddObject* token);
        void visit(const SubtractObject* token);
        void visit(const MultiplyObject* token);
        void visit(const DivideObject* token);
        void visit(const PowerObject* token);
        void visit(const EqualsObject* token);
        void visit(const LessObject* token);
        void visit(const GreaterObject* token);
        void visit(const LessEqualObject* token);
        void visit(const GreaterEqualObject* token);
        void visit(const ParenthesizedExpression* token);
        void visit(const Assign* token);
        void visit(const Call* token);
        void visit(const Define* token);
        void visit(const Program* token);

        Scope* currentScope;

        const std::string sourcePath;

    private:
        std::string outputPath;

        BytecodeResolver* resolver = new BytecodeResolver();

        Utils* utils;

    };
}
