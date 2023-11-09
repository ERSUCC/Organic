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

struct List : public Token
{
    std::vector<Token*> items;
};

struct Instruction : public Token {};

struct Assign : public Instruction
{
    Assign(std::string variable, Token* value);

    void accept(ProgramVisitor* visitor) override;

    std::string variable;
    Token* value;
};

struct CreateAudioSource : public Instruction
{
    CreateAudioSource(Token* volume, Token* pan, List* effects);

    Token* volume;
    Token* pan;
    List* effects;
};

struct CreateOscillator : public CreateAudioSource
{
    CreateOscillator(Token* volume, Token* pan, Token* frequency, List* effects);

    Token* frequency;
};

struct CreateSine : public CreateOscillator
{
    CreateSine(Token* volume, Token* pan, Token* frequency, List* effects);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateSquare : public CreateOscillator
{
    CreateSquare(Token* volume, Token* pan, Token* frequency, List* effects);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateSaw : public CreateOscillator
{
    CreateSaw(Token* volume, Token* pan, Token* frequency, List* effects);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateTriangle : public CreateOscillator
{
    CreateTriangle(Token* volume, Token* pan, Token* frequency, List* effects);

    void accept(ProgramVisitor* visitor) override;
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

struct CreateEffect : public Instruction
{
    CreateEffect(Token* mix);

    Token* mix;
};

struct CreateDelay : public CreateEffect
{
    CreateDelay(Token* mix, Token* delay, Token* feedback);

    void accept(ProgramVisitor* visitor) override;

    Token* delay;
    Token* feedback;
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
    void visit(CreateTriangle* token);
    void visit(CreateHold* token);
    void visit(CreateSweep* token);
    void visit(CreateLFO* token);
    void visit(CreateDelay* token);
    void visit(Program* token);

    std::vector<AudioSource*> sources;
    ControllerManager* controllerManager;
    EventQueue* eventQueue;

private:
    void visitWithSlot(Token* token, void* slot);

    std::stack<void*> slots;

    std::unordered_map<std::string, Token*> variables;

};