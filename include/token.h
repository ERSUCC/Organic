#pragma once

#include <vector>
#include <string>
#include <stack>
#include <unordered_map>

#include "audiosource.h"
#include "controller.h"
#include "event.h"

struct ProgramVisitor;

struct Token
{
    Token(int line, int character);

    virtual void accept(ProgramVisitor* visitor) {}

    int line;
    int character;
};

struct Constant : public Token
{
    Constant(int line, int character, double value);

    void accept(ProgramVisitor* visitor) override;

    double value;
};

struct Name : public Token
{
    Name(int line, int character, std::string name);

    std::string name;
};

struct VariableRef : public Name
{
    VariableRef(int line, int character, std::string name);

    void accept(ProgramVisitor* visitor) override;
};

struct Argument : public Token
{
    Argument(int line, int character, Name* name, Token* value);

    Name* name;
    Token* value;
};

struct List : public Token
{
    List(int line, int character);

    std::vector<Token*> items;
};

struct CreateValueCombination : public Token
{
    CreateValueCombination(int line, int character, Token* value1, Token* value2);

    Token* value1;
    Token* value2;
};

struct CreateValueAdd : public CreateValueCombination
{
    CreateValueAdd(int line, int character, Token* value1, Token* value2);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateValueSubtract : public CreateValueCombination
{
    CreateValueSubtract(int line, int character, Token* value1, Token* value2);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateValueMultiply : public CreateValueCombination
{
    CreateValueMultiply(int line, int character, Token* value1, Token* value2);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateValueDivide : public CreateValueCombination
{
    CreateValueDivide(int line, int character, Token* value1, Token* value2);

    void accept(ProgramVisitor* visitor) override;
};

struct Instruction : public Token
{
    Instruction(int line, int character);
};

struct Assign : public Instruction
{
    Assign(int line, int character, std::string variable, Token* value);

    void accept(ProgramVisitor* visitor) override;

    std::string variable;
    Token* value;
};

struct CreateAudioSource : public Instruction
{
    CreateAudioSource(int line, int character, Token* volume, Token* pan, List* effects);

    Token* volume;
    Token* pan;
    List* effects;
};

struct CreateOscillator : public CreateAudioSource
{
    CreateOscillator(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects);

    Token* frequency;
};

struct CreateSine : public CreateOscillator
{
    CreateSine(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateSquare : public CreateOscillator
{
    CreateSquare(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateSaw : public CreateOscillator
{
    CreateSaw(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateTriangle : public CreateOscillator
{
    CreateTriangle(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects);

    void accept(ProgramVisitor* visitor) override;
};

struct CreateHold : public Instruction
{
    CreateHold(int line, int character, Token* value, Token* length);

    void accept(ProgramVisitor* visitor) override;

    Token* value;
    Token* length;
};

struct CreateSweep : public Instruction
{
    CreateSweep(int line, int character, Token* repeats, Token* from, Token* to, Token* length);

    void accept(ProgramVisitor* visitor) override;

    Token* repeats;
    Token* from;
    Token* to;
    Token* length;
};

struct CreateLFO : public Instruction
{
    CreateLFO(int line, int character, Token* repeats, Token* from, Token* to, Token* length);

    void accept(ProgramVisitor* visitor) override;

    Token* repeats;
    Token* from;
    Token* to;
    Token* length;
};

struct GroupOrder : public Token
{
    GroupOrder(int line, int character, Sequence::OrderEnum order);

    void accept(ProgramVisitor* visitor) override;

    Sequence::OrderEnum order;
};

struct CreateSequence : public Instruction
{
    CreateSequence(int line, int character, Token* repeats, List* controllers, Token* order);

    void accept(ProgramVisitor* visitor) override;

    Token* repeats;
    List* controllers;
    Token* order;
};

struct RandomType : public Token
{
    RandomType(int line, int character, Random::TypeEnum type);

    void accept(ProgramVisitor* visitor) override;
    
    Random::TypeEnum type;
};

struct CreateRandom : public Instruction
{
    CreateRandom(int line, int character, Token* repeats, Token* from, Token* to, Token* length, RandomType* type);

    void accept(ProgramVisitor* visitor) override;

    Token* repeats;
    Token* from;
    Token* to;
    Token* length;
    RandomType* type;
};

struct CreateEffect : public Instruction
{
    CreateEffect(int line, int character, Token* mix);

    Token* mix;
};

struct CreateDelay : public CreateEffect
{
    CreateDelay(int line, int character, Token* mix, Token* delay, Token* feedback);

    void accept(ProgramVisitor* visitor) override;

    Token* delay;
    Token* feedback;
};

struct Program
{
    void accept(ProgramVisitor* visitor);

    std::vector<Instruction*> instructions;
};

struct ProgramVisitor
{
    void visit(Constant* token);
    void visit(VariableRef* token);
    void visit(CreateValueAdd* token);
    void visit(CreateValueSubtract* token);
    void visit(CreateValueMultiply* token);
    void visit(CreateValueDivide* token);
    void visit(Assign* token);
    void visit(CreateSine* token);
    void visit(CreateSquare* token);
    void visit(CreateSaw* token);
    void visit(CreateTriangle* token);
    void visit(CreateHold* token);
    void visit(CreateSweep* token);
    void visit(CreateLFO* token);
    void visit(GroupOrder* token);
    void visit(CreateSequence* token);
    void visit(RandomType* token);
    void visit(CreateRandom* token);
    void visit(CreateDelay* token);
    void visit(Program* token);

    std::vector<AudioSource*> sources;
    EventQueue* eventQueue = new EventQueue();

private:
    void visitWithSlot(Token* token, Object** slot);
    void setLastSlot(Object* value);

    std::stack<Object**> slots;

    std::unordered_map<std::string, Variable*> variables;

};
