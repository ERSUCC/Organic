#pragma once

#include <filesystem>
#include <fstream>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

#include "bytecode.h"

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
        Token(const ParserLocation location, const std::string str);

        virtual void accept(BytecodeTransformer* visitor) const;

        const ParserLocation location;

        const std::string str;
    };

    struct OpenParenthesis : public Token
    {
        OpenParenthesis(const ParserLocation location);
    };

    struct CloseParenthesis : public Token
    {
        CloseParenthesis(const ParserLocation location);
    };

    struct OpenCurlyBracket : public Token
    {
        OpenCurlyBracket(const ParserLocation location);
    };

    struct CloseCurlyBracket : public Token
    {
        CloseCurlyBracket(const ParserLocation location);
    };

    struct Colon : public Token
    {
        Colon(const ParserLocation location);
    };

    struct Comma : public Token
    {
        Comma(const ParserLocation location);
    };

    struct Equals : public Token
    {
        Equals(const ParserLocation location);
    };

    struct Operator : public Token
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

    struct String : public Token
    {
        String(const ParserLocation location, const std::string str);
    };

    struct Value : public Token
    {
        Value(const ParserLocation location, const std::string str, const double value);

        void accept(BytecodeTransformer* visitor) const override;

        const double value;
    };

    struct NamedConstant : public Token
    {
        NamedConstant(const ParserLocation location, const std::string constant);

        void accept(BytecodeTransformer* visitor) const override;
    };

    struct Variable : public Token
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

        void get(const std::string name, Token* defaultValue, BytecodeTransformer* visitor);

        void confirmEmpty() const;

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

    struct AddObject : public Token
    {
        AddObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct SubtractObject : public Token
    {
        SubtractObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct MultiplyObject : public Token
    {
        MultiplyObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct DivideObject : public Token
    {
        DivideObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct PowerObject : public Token
    {
        PowerObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct EqualsObject : public Token
    {
        EqualsObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct LessObject : public Token
    {
        LessObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct GreaterObject : public Token
    {
        GreaterObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct LessEqualObject : public Token
    {
        LessEqualObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
        const Token* value2;
    };

    struct GreaterEqualObject : public Token
    {
        GreaterEqualObject(const ParserLocation location, const Token* value1, const Token* value2);

        void accept(BytecodeTransformer* visitor) const override;

        const Token* value1;
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
        Instruction(const ParserLocation location, const std::string str);
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

    struct CodeBlock : public Token
    {
        CodeBlock(const ParserLocation location, const std::vector<const Instruction*> instructions);

        void accept(BytecodeTransformer* visitor) const override;

        const std::vector<const Instruction*> instructions;
    };

    struct Scope
    {
        Scope(Scope* parent = nullptr);

        bool getVariable(const std::string name);
        void addVariable(const std::string name);

        void checkVariableUses() const;

        BytecodeTransformer* visitor;

        Scope* parent;

        BytecodeBlock* block = new BytecodeBlock();

    private:
        std::unordered_set<std::string> variables;
        std::unordered_set<std::string> variablesUsed;

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
        void visit(const CodeBlock* token);
        void visit(const Program* token);

        Scope* currentScope;

    private:
        const std::string sourcePath;

        std::string outputPath;

        BytecodeResolver* resolver = new BytecodeResolver();

    };
}
