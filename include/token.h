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

    struct Token
    {
        Token(const ParserLocation location);

        virtual void accept(BytecodeTransformer* visitor) const;

        const ParserLocation location;
    };

    struct BasicToken : public Token
    {
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

        void accept(BytecodeTransformer* visitor) const override;

        const double value;
    };

    struct NamedConstant : public BasicToken
    {
        NamedConstant(const ParserLocation location, const std::string constant);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct Variable : public BasicToken
    {
        Variable(const ParserLocation location, const std::string variable);

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

        void get(const std::string name, BytecodeTransformer* visitor);

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

        void accept(BytecodeTransformer* visitor) const override;

        const std::vector<const Token*> values;
    };

    struct OperatorObject : public Token
    {
        OperatorObject(const ParserLocation location, const Token* value1, const Token* value2);

        const Token* value1;
        const Token* value2;
    };

    struct AddObject : public OperatorObject
    {
        AddObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct SubtractObject : public OperatorObject
    {
        SubtractObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct MultiplyObject : public OperatorObject
    {
        MultiplyObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct DivideObject : public OperatorObject
    {
        DivideObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct PowerObject : public OperatorObject
    {
        PowerObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct EqualsObject : public OperatorObject
    {
        EqualsObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct LessObject : public OperatorObject
    {
        LessObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct GreaterObject : public OperatorObject
    {
        GreaterObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct LessEqualObject : public OperatorObject
    {
        LessEqualObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct GreaterEqualObject : public OperatorObject
    {
        GreaterEqualObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;
        const Token* value2;
    };

    struct ParenthesizedExpression : public Token
    {
        ParenthesizedExpression(const ParserLocation, const Token* value);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value;
    };

    struct Instruction : public Token
    {
        Instruction(const ParserLocation location);
    };

    struct Assign : public Instruction
    {
        Assign(const ParserLocation location, const std::string variable, const Token* value);

        void accept(BytecodeTransformer* visitor) const override;

        const std::string variable;

        const Token* value;
    };

    struct Call : public Instruction
    {
        Call(const ParserLocation location, const std::string name, ArgumentList* arguments);

        void accept(BytecodeTransformer* visitor) const override;

        const std::string name;

        ArgumentList* arguments;
    };

    struct Define : public Token
    {
        Define(const ParserLocation location, const std::string name, const std::vector<std::string> inputs, const std::vector<const Instruction*> instructions);

        void accept(BytecodeTransformer* visitor) const override;

        const std::string name;

        const std::vector<std::string> inputs;

        const std::vector<const Instruction*> instructions;
    };

    struct Scope
    {
        Scope(Scope* parent = nullptr, const std::string currentFunction = "", const std::vector<std::string> inputs = std::vector<std::string>());

        std::optional<unsigned char> getVariable(const std::string name);
        void addVariable(const std::string name);

        BytecodeBlock* getFunction(const std::string name);
        void addFunction(const std::string name, BytecodeBlock* body);

        std::optional<unsigned char> getInput(const std::string input);

        bool checkRecursive(const std::string function) const;

        void checkUses() const;

        Scope* parent;

        const std::string currentFunction;

        BytecodeBlock* block;

    private:
        std::unordered_map<std::string, unsigned char> variables;
        std::unordered_set<std::string> variablesUsed;

        std::unordered_map<std::string, BytecodeBlock*> functions;
        std::unordered_set<std::string> functionsUsed;

        std::unordered_map<std::string, unsigned char> inputs;
        std::unordered_set<std::string> inputsUsed;

    };

    struct Program : public Token
    {
        Program(const std::vector<const Instruction*> instructions);

        void accept(BytecodeTransformer* visitor) const override;

        const std::vector<const Instruction*> instructions;
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

    private:
        const std::string sourcePath;

        std::string outputPath;

        BytecodeResolver* resolver = new BytecodeResolver();

        Utils* utils;

    };
}
