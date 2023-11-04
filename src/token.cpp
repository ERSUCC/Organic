#include "../include/token.h"

std::string Token::toString()
{
    return "";
}

Constant::Constant(double value) : value(value) {}

std::string Constant::toString()
{
    return std::to_string(value);
}

void Constant::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Name::Name(std::string name) : name(name) {}

std::string Name::toString()
{
    return name;
}

CreateSine::CreateSine(Token* volume, Token* pan, Token* frequency) : volume(volume), pan(pan), frequency(frequency) {}

std::string CreateSine::toString()
{
    return "sine(" + volume->toString() + ", " + pan->toString() + ", " + frequency->toString() + ")";
}

void CreateSine::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

std::string Program::toString()
{
    std::string s;

    for (Instruction* instruction : instructions)
    {
        s += instruction->toString() + "\n";
    }

    return s;
}

ProgramVisitor::ProgramVisitor()
{
    controllerManager = new ControllerManager();
    eventQueue = new EventQueue();
}

void ProgramVisitor::visit(Constant* token)
{
    *slots.top() = token->value;
}

void ProgramVisitor::visit(CreateSine* token)
{
    Sine* sine = new Sine(1, 0, 0);

    prepareForVisit(token->volume, &sine->volume);
    prepareForVisit(token->pan, &sine->pan);
    prepareForVisit(token->frequency, &sine->frequency);

    sources.push_back(sine);
}

void ProgramVisitor::visit(Program* token)
{
    for (Instruction* instruction : token->instructions)
    {
        instruction->accept(this);
    }
}

void ProgramVisitor::prepareForVisit(Token* token, Parameter* slot)
{
    slots.push(slot);

    token->accept(this);

    slots.pop();
}