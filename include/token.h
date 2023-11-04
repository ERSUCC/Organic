#pragma once

#include <vector>
#include <string>
#include <stack>

#include "audiosource.h"
#include "event.h"
#include "parameter.h"

struct ProgramVisitor;

struct Token
{
    virtual std::string toString();

    virtual void accept(ProgramVisitor* visitor) {}
};

struct Constant : public Token
{
    Constant(double value);

    std::string toString() override;

    void accept(ProgramVisitor* visitor) override;

    double value;
};

struct Name : public Token
{
    Name(std::string name);

    std::string toString() override;

    std::string name;
};

struct Argument : public Token
{
    Argument(Name* name, Token* value);

    std::string toString() override;

    Name* name;
    Token* value;
};

struct Instruction : public Token {};

struct CreateSine : public Instruction
{
    CreateSine(Token* volume, Token* pan, Token* frequency);

    std::string toString() override;

    void accept(ProgramVisitor* visitor) override;

    Token* volume;
    Token* pan;
    Token* frequency;
};

struct CreateLFO : public Instruction
{
    CreateLFO(Token* repeats, Token* floor, Token* ceiling, Token* rate);

    std::string toString() override;

    void accept(ProgramVisitor* visitor) override;

    Token* repeats;
    Token* floor;
    Token* ceiling;
    Token* rate;
};

struct Program : public Token
{
    std::string toString() override;

    std::vector<Instruction*> instructions;
};

struct ProgramVisitor
{
    ProgramVisitor();

    void visit(Constant* token);
    void visit(CreateSine* token);
    void visit(CreateLFO* token);
    void visit(Program* token);

    std::vector<AudioSource*> sources;
    ControllerManager* controllerManager;
    EventQueue* eventQueue;

private:
    void prepareForVisit(Token* token, Parameter* slot);

    std::stack<Parameter*> slots;

};