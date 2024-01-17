#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "audiosource.h"
#include "controller.h"
#include "effect.h"
#include "event.h"

struct ProgramVisitor;

struct Token
{
    Token(const int line, const int character, const std::string str = "");

    virtual std::string string() const;

    virtual Token* copy() const;

    virtual Object* accept(ProgramVisitor* visitor) const;

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

    Token* copy() const override;
};

struct CloseParenthesis : public Token
{
    CloseParenthesis(const int line, const int character);

    Token* copy() const override;
};

struct Colon : public Token
{
    Colon(const int line, const int character);

    Token* copy() const override;
};

struct Comma : public Token
{
    Comma(const int line, const int character);

    Token* copy() const override;
};

struct Equals : public Token
{
    Equals(const int line, const int character);

    Token* copy() const override;
};

struct Operator : public Token
{
    Operator(const int line, const int character, std::string str);
};

struct AddToken : public Operator
{
    AddToken(const int line, const int character);

    Token* copy() const override;
};

struct SubtractToken : public Operator
{
    SubtractToken(const int line, const int character);

    Token* copy() const override;
};

struct MultiplyToken : public Operator
{
    MultiplyToken(const int line, const int character);

    Token* copy() const override;
};

struct DivideToken : public Operator
{
    DivideToken(const int line, const int character);

    Token* copy() const override;
};

struct Name : public Token
{
    Name(const int line, const int character, const std::string name, const bool value = false);

    Token* copy() const override;

    Object* accept(ProgramVisitor* visitor) const override;

    const std::string name;

    const bool value;
};

struct Constant : public Token
{
    Constant(const int line, const int character, const std::string str);

    Token* copy() const override;

    Object* accept(ProgramVisitor* visitor) const override;

    const double value;
};

struct Argument : public Token
{
    Argument(const int line, const int character, const std::string name, const Token* value);

    Token* copy() const override;

    const std::string name;
    const Token* value;
};

struct ArgumentList
{
    ArgumentList(const std::vector<Argument*> arguments, const std::string name, const std::string path);

    Object* get(const std::string name, Object* defaultValue, ProgramVisitor* visitor);

    void confirmEmpty() const;

    std::string string() const;

private:
    std::unordered_map<std::string, const Argument*> arguments;
    std::vector<Argument*> argumentsOrdered;

    const std::string name;
    const std::string path;

};

struct ListToken : public Token
{
    ListToken(const int line, const int character, const std::vector<Token*> values);

    Token* copy() const override;

    std::string string() const override;

    Object* accept(ProgramVisitor* visitor) const override;

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

    Token* copy() const override;

    Object* accept(ProgramVisitor* visitor) const override;
};

struct Subtract : public Combine
{
    Subtract(const Token* value1, const Token* value2);

    Token* copy() const override;

    Object* accept(ProgramVisitor* visitor) const override;
};

struct Multiply : public Combine
{
    Multiply(const Token* value1, const Token* value2);

    Token* copy() const override;

    Object* accept(ProgramVisitor* visitor) const override;
};

struct Divide : public Combine
{
    Divide(const Token* value1, const Token* value2);

    Token* copy() const override;

    Object* accept(ProgramVisitor* visitor) const override;
};

struct Instruction : public Token
{
    Instruction(const int line, const int character, const std::string str = "");
};

struct Assign : public Instruction
{
    Assign(const Name* variable, const Token* value);

    Token* copy() const override;

    Object* accept(ProgramVisitor* visitor) const override;

    const Name* variable;
    const Token* value;
};

struct Call : public Instruction
{
    Call(const Name* name, ArgumentList* arguments);

    Token* copy() const override;

    std::string string() const override;

    Object* accept(ProgramVisitor* visitor) const override;

    const Name* name;
    ArgumentList* arguments;
};

struct Program : public Token
{
    Program(const std::vector<Instruction*> instructions);

    Token* copy() const override;

    std::string string() const override;
    
    Object* accept(ProgramVisitor* visitor) const override;

    const std::vector<Instruction*> instructions;
};

struct ProgramVisitor
{
    ProgramVisitor(const std::string sourcePath);

    Object* visit(const Name* token);
    Object* visit(const Constant* token);
    Object* visit(const ListToken* token);
    Object* visit(const Add* token);
    Object* visit(const Subtract* token);
    Object* visit(const Multiply* token);
    Object* visit(const Divide* token);
    Object* visit(const Assign* token);
    Object* visit(const Call* token);
    Object* visit(const Program* token);

    std::vector<AudioSource*> sources;
    EventQueue* eventQueue = new EventQueue();

private:
    template <typename T> List<T>* getList(Object* object) const;

    double getFrequency(const double note) const;

    std::unordered_map<std::string, Variable*> variables;
    std::unordered_set<std::string> variablesUsed;

    const std::string path;

    std::string currentVariable;

};
