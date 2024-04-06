#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "audiosource.h"
#include "bytecode.h"
#include "controller.h"
#include "effect.h"
#include "event.h"
#include "object.h"

struct BytecodeTransformer;

struct Token
{
    Token(const int line, const int character, const std::string str = "");

    virtual std::string string() const;

    virtual void accept(BytecodeTransformer* visitor) const;

    const int line;
    const int character;

protected:
    const std::string str;

};

struct TokenRange
{
    TokenRange(const int start, const int end, Token* token);

    const int start;
    const int end;
    
    Token* token;
};

struct OpenParenthesis : public Token
{
    OpenParenthesis(const int line, const int character);
};

struct CloseParenthesis : public Token
{
    CloseParenthesis(const int line, const int character);
};

struct OpenCurlyBracket : public Token
{
    OpenCurlyBracket(const int line, const int character);
};

struct CloseCurlyBracket : public Token
{
    CloseCurlyBracket(const int line, const int character);
};

struct Colon : public Token
{
    Colon(const int line, const int character);
};

struct Comma : public Token
{
    Comma(const int line, const int character);
};

struct Equals : public Token
{
    Equals(const int line, const int character);
};

struct Operator : public Token
{
    Operator(const int line, const int character, std::string str);
};

struct AddToken : public Operator
{
    AddToken(const int line, const int character);
};

struct SubtractToken : public Operator
{
    SubtractToken(const int line, const int character);
};

struct MultiplyToken : public Operator
{
    MultiplyToken(const int line, const int character);
};

struct DivideToken : public Operator
{
    DivideToken(const int line, const int character);
};

struct Name : public Token
{
    Name(const int line, const int character, const std::string name, const bool value = false);

    void accept(BytecodeTransformer* visitor) const override;

    const std::string name;

    const bool value;
};

struct Constant : public Token
{
    Constant(const int line, const int character, const std::string str);

    void accept(BytecodeTransformer* visitor) const override;

    const double value;
};

struct Argument : public Token
{
    Argument(const int line, const int character, const std::string name, const Token* value);

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
    ListToken(const int line, const int character, const std::vector<Token*> values);

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

struct Instruction : public Token
{
    Instruction(const int line, const int character, const std::string str = "");
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
    CodeBlock(const int line, const int character, const std::vector<Instruction*> instructions);

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
