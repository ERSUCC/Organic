#pragma once

#include <vector>
#include <string>
#include <stack>
#include <unordered_map>

#include "audiosource.h"
#include "event.h"
#include "parameter.h"

struct ProgramVisitor;

struct Token
{
    virtual void accept(ProgramVisitor* visitor) {}
};

struct Constant : public Token
{
    Constant(double value);

    void accept(ProgramVisitor* visitor) override;

    double value;
};

struct Name : public Token
{
    Name(std::string name);

    std::string name;
};

struct Variable : public Name
{
    Variable(std::string name);

    void accept(ProgramVisitor* visitor) override;
};

struct Argument : public Token
{
    Argument(Name* name, Token* value);

    Name* name;
    Token* value;
};

struct Instruction : public Token {};

struct Assign : public Instruction
{
    Assign(std::string variable, Token* value);

    void accept(ProgramVisitor* visitor) override;

    std::string variable;
    Token* value;
};

struct CreateSine : public Instruction
{
    CreateSine(Token* volume, Token* pan, Token* frequency);

    void accept(ProgramVisitor* visitor) override;

    Token* volume;
    Token* pan;
    Token* frequency;
};

struct CreateSquare : public Instruction
{
    CreateSquare(Token* volume, Token* pan, Token* frequency);

    void accept(ProgramVisitor* visitor) override;

    Token* volume;
    Token* pan;
    Token* frequency;
};

struct CreateSaw : public Instruction
{
    CreateSaw(Token* volume, Token* pan, Token* frequency);

    void accept(ProgramVisitor* visitor) override;

    Token* volume;
    Token* pan;
    Token* frequency;
};

struct CreateHold : public Instruction
{
    CreateHold(Token* value, Token* length);

    void accept(ProgramVisitor* visitor) override;

    Token* value;
    Token* length;
};

struct CreateSweep : public Instruction
{
    CreateSweep(Token* repeats, Token* from, Token* to, Token* length);

    void accept(ProgramVisitor* visitor) override;

    Token* repeats;
    Token* from;
    Token* to;
    Token* length;
};

struct CreateLFO : public Instruction
{
    CreateLFO(Token* repeats, Token* from, Token* to, Token* length);

    void accept(ProgramVisitor* visitor) override;

    Token* repeats;
    Token* from;
    Token* to;
    Token* length;
};

struct Program : public Token
{
    std::vector<Instruction*> instructions;
};

struct ProgramVisitor
{
    ProgramVisitor();

    void visit(Constant* token);
    void visit(Variable* token);
    void visit(Assign* token);
    void visit(CreateSine* token);
    void visit(CreateSquare* token);
    void visit(CreateSaw* token);
    void visit(CreateHold* token);
    void visit(CreateSweep* token);
    void visit(CreateLFO* token);
    void visit(Program* token);

    std::vector<AudioSource*> sources;
    ControllerManager* controllerManager;
    EventQueue* eventQueue;

private:
    void visitWithParameter(Token* token, Parameter* slot);

    std::stack<Parameter*> slots;

    std::unordered_map<std::string, Token*> variables;

};