#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "audiosource.h"
#include "controller.h"
#include "event.h"

struct ProgramVisitor;

struct Token
{
    Token(int line, int character);

    virtual Object* accept(ProgramVisitor* visitor);

    int line;
    int character;
};

struct Constant : public Token
{
    Constant(int line, int character, double value);

    Object* accept(ProgramVisitor* visitor) override;

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

    Object* accept(ProgramVisitor* visitor) override;
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

    Object* accept(ProgramVisitor* visitor) override;
};

struct CreateValueSubtract : public CreateValueCombination
{
    CreateValueSubtract(int line, int character, Token* value1, Token* value2);

    Object* accept(ProgramVisitor* visitor) override;
};

struct CreateValueMultiply : public CreateValueCombination
{
    CreateValueMultiply(int line, int character, Token* value1, Token* value2);

    Object* accept(ProgramVisitor* visitor) override;
};

struct CreateValueDivide : public CreateValueCombination
{
    CreateValueDivide(int line, int character, Token* value1, Token* value2);

    Object* accept(ProgramVisitor* visitor) override;
};

struct Instruction : public Token
{
    Instruction(int line, int character);
};

struct Assign : public Instruction
{
    Assign(int line, int character, std::string variable, Token* value);

    Object* accept(ProgramVisitor* visitor) override;

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

    Object* accept(ProgramVisitor* visitor) override;
};

struct CreateSquare : public CreateOscillator
{
    CreateSquare(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects);

    Object* accept(ProgramVisitor* visitor) override;
};

struct CreateSaw : public CreateOscillator
{
    CreateSaw(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects);

    Object* accept(ProgramVisitor* visitor) override;
};

struct CreateTriangle : public CreateOscillator
{
    CreateTriangle(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects);

    Object* accept(ProgramVisitor* visitor) override;
};

struct CreateHold : public Instruction
{
    CreateHold(int line, int character, Token* value, Token* length);

    Object* accept(ProgramVisitor* visitor) override;

    Token* value;
    Token* length;
};

struct CreateSweep : public Instruction
{
    CreateSweep(int line, int character, Token* from, Token* to, Token* length);

    Object* accept(ProgramVisitor* visitor) override;

    Token* from;
    Token* to;
    Token* length;
};

struct CreateLFO : public Instruction
{
    CreateLFO(int line, int character, Token* from, Token* to, Token* length);

    Object* accept(ProgramVisitor* visitor) override;

    Token* from;
    Token* to;
    Token* length;
};

struct GroupOrder : public Token
{
    GroupOrder(int line, int character, Sequence::OrderEnum order);

    Object* accept(ProgramVisitor* visitor) override;

    Sequence::OrderEnum order;
};

struct CreateSequence : public Instruction
{
    CreateSequence(int line, int character, List* controllers, Token* order);

    Object* accept(ProgramVisitor* visitor) override;

    List* controllers;
    Token* order;
};

struct CreateRepeat : public Instruction
{
    CreateRepeat(int line, int character, Token* value, Token* repeats);

    Object* accept(ProgramVisitor* visitor) override;

    Token* value;
    Token* repeats;
};

struct RandomType : public Token
{
    RandomType(int line, int character, Random::TypeEnum type);

    Object* accept(ProgramVisitor* visitor) override;
    
    Random::TypeEnum type;
};

struct CreateRandom : public Instruction
{
    CreateRandom(int line, int character, Token* from, Token* to, Token* length, RandomType* type);

    Object* accept(ProgramVisitor* visitor) override;

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

    Object* accept(ProgramVisitor* visitor) override;

    Token* delay;
    Token* feedback;
};

struct Program
{
    Object* accept(ProgramVisitor* visitor);

    std::vector<Instruction*> instructions;
};

struct ProgramVisitor
{
    ProgramVisitor(const std::string sourcePath);

    Object* visit(Constant* token);
    Object* visit(VariableRef* token);
    Object* visit(CreateValueAdd* token);
    Object* visit(CreateValueSubtract* token);
    Object* visit(CreateValueMultiply* token);
    Object* visit(CreateValueDivide* token);
    Object* visit(Assign* token);
    Object* visit(CreateSine* token);
    Object* visit(CreateSquare* token);
    Object* visit(CreateSaw* token);
    Object* visit(CreateTriangle* token);
    Object* visit(CreateHold* token);
    Object* visit(CreateSweep* token);
    Object* visit(CreateLFO* token);
    Object* visit(GroupOrder* token);
    Object* visit(CreateSequence* token);
    Object* visit(RandomType* token);
    Object* visit(CreateRandom* token);
    Object* visit(CreateRepeat* token);
    Object* visit(CreateDelay* token);
    Object* visit(Program* token);

    std::vector<AudioSource*> sources;
    EventQueue* eventQueue = new EventQueue();

private:
    std::unordered_map<std::string, Variable*> variables;

    const std::string sourcePath;

};
