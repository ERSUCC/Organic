#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "bytecode.h"

struct BytecodeTransformer;

struct Token
{
    Token(const unsigned int line, const unsigned int character, const std::string str = "");

    virtual std::string string() const;

    virtual void accept(BytecodeTransformer* visitor) const;

    const unsigned int line;
    const unsigned int character;

protected:
    const std::string str;

};

struct TokenRange
{
    TokenRange(const unsigned int start, const unsigned int end, Token* token);

    const unsigned int start;
    const unsigned int end;
    
    Token* token;
};

struct OpenParenthesis : public Token
{
    OpenParenthesis(const unsigned int line, const unsigned int character);
};

struct CloseParenthesis : public Token
{
    CloseParenthesis(const unsigned int line, const unsigned int character);
};

struct OpenCurlyBracket : public Token
{
    OpenCurlyBracket(const unsigned int line, const unsigned int character);
};

struct CloseCurlyBracket : public Token
{
    CloseCurlyBracket(const unsigned int line, const unsigned int character);
};

struct Colon : public Token
{
    Colon(const unsigned int line, const unsigned int character);
};

struct Comma : public Token
{
    Comma(const unsigned int line, const unsigned int character);
};

struct EqualToken : public Token
{
    EqualToken(const unsigned int line, const unsigned int character);
};

struct Operator : public Token
{
    Operator(const unsigned int line, const unsigned int character, std::string str);
};

struct AddToken : public Operator
{
    AddToken(const unsigned int line, const unsigned int character);
};

struct SubtractToken : public Operator
{
    SubtractToken(const unsigned int line, const unsigned int character);
};

struct MultiplyToken : public Operator
{
    MultiplyToken(const unsigned int line, const unsigned int character);
};

struct DivideToken : public Operator
{
    DivideToken(const unsigned int line, const unsigned int character);
};

struct DoubleEqualToken : public Operator
{
    DoubleEqualToken(const unsigned int line, const unsigned int character);
};

struct LessToken : public Operator
{
    LessToken(const unsigned int line, const unsigned int character);
};

struct GreaterToken : public Operator
{
    GreaterToken(const unsigned int line, const unsigned int character);
};

struct LessEqualToken : public Operator
{
    LessEqualToken(const unsigned int line, const unsigned int character);
};

struct GreaterEqualToken : public Operator
{
    GreaterEqualToken(const unsigned int line, const unsigned int character);
};

struct Name : public Token
{
    Name(const unsigned int line, const unsigned int character, const std::string name, const bool value = false);

    void accept(BytecodeTransformer* visitor) const override;

    const std::string name;

    const bool value;
};

struct Constant : public Token
{
    Constant(const unsigned int line, const unsigned int character, const std::string str);

    void accept(BytecodeTransformer* visitor) const override;

    const double value;
};

struct Argument : public Token
{
    Argument(const unsigned int line, const unsigned int character, const std::string name, const Token* value);

    const std::string name;
    const Token* value;
};

struct ArgumentList
{
    ArgumentList(const std::vector<Argument*> arguments, const std::string name, const std::string path);

    void get(const std::string name, Token* defaultValue, BytecodeTransformer* visitor);

    void confirmEmpty() const;

    std::string string() const;

private:
    std::vector<Argument*> arguments;

    const std::string name;
    const std::string path;

};

struct ListToken : public Token
{
    ListToken(const unsigned int line, const unsigned int character, const std::vector<Token*> values);

    std::string string() const override;

    void accept(BytecodeTransformer* visitor) const override;

    const std::vector<Token*> values;
};

struct Combine : public Token
{
    Combine(const Token* value1, const Token* value2, const std::string op);

    const Token* value1;
    const Token* value2;
};

struct Add : public Combine
{
    Add(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct Subtract : public Combine
{
    Subtract(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct Multiply : public Combine
{
    Multiply(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct Divide : public Combine
{
    Divide(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct Comparison : public Combine
{
    Comparison(const Token* value1, const Token* value2, const std::string op);
};

struct Equal : public Comparison
{
    Equal(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct Less : public Comparison
{
    Less(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct Greater : public Comparison
{
    Greater(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct LessEqual : public Comparison
{
    LessEqual(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct GreaterEqual : public Comparison
{
    GreaterEqual(const Token* value1, const Token* value2);

    void accept(BytecodeTransformer* visitor) const override;
};

struct Instruction : public Token
{
    Instruction(const unsigned int line, const unsigned int character, const std::string str = "");
};

struct Assign : public Instruction
{
    Assign(const Name* variable, const Token* value);

    void accept(BytecodeTransformer* visitor) const override;

    const Name* variable;
    const Token* value;
};

struct Call : public Instruction
{
    Call(const Name* name, ArgumentList* arguments);

    std::string string() const override;

    void accept(BytecodeTransformer* visitor) const override;

    const Name* name;
    ArgumentList* arguments;
};

struct CodeBlock : public Token
{
    CodeBlock(const unsigned int line, const unsigned int character, const std::vector<Instruction*> instructions);

    std::string string() const override;

    void accept(BytecodeTransformer* visitor) const override;

    const std::vector<Instruction*> instructions;
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
    Program(const std::vector<Instruction*> instructions);

    std::string string() const override;
    
    void accept(BytecodeTransformer* visitor) const override;

    const std::vector<Instruction*> instructions;
};

struct BytecodeTransformer
{
    BytecodeTransformer(const std::string sourcePath);

    std::string transform(const Program* program);

    void visit(const Name* token);
    void visit(const Constant* token);
    void visit(const ListToken* token);
    void visit(const Add* token);
    void visit(const Subtract* token);
    void visit(const Multiply* token);
    void visit(const Divide* token);
    void visit(const Equal* token);
    void visit(const Less* token);
    void visit(const Greater* token);
    void visit(const LessEqual* token);
    void visit(const GreaterEqual* token);
    void visit(const Assign* token);
    void visit(const Call* token);
    void visit(const CodeBlock* token);
    void visit(const Program* token);

    Scope* currentScope;

private:
    double getFrequency(const double note) const;

    const std::string sourcePath;

    std::string outputPath;

    std::string currentVariable;

    BytecodeResolver* resolver = new BytecodeResolver();

};
