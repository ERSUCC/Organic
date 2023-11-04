#include "../include/token.h"

Constant::Constant(double value) : value(value) {}

void Constant::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Name::Name(std::string name) : name(name) {}

Argument::Argument(Name* name, Token* value) : name(name), value(value) {}

CreateSine::CreateSine(Token* volume, Token* pan, Token* frequency) :
    volume(volume), pan(pan), frequency(frequency) {}

void CreateSine::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSquare::CreateSquare(Token* volume, Token* pan, Token* frequency) :
    volume(volume), pan(pan), frequency(frequency) {}

void CreateSquare::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSaw::CreateSaw(Token* volume, Token* pan, Token* frequency) :
    volume(volume), pan(pan), frequency(frequency) {}

void CreateSaw::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateHold::CreateHold(Token* value, Token* length) : value(value), length(length) {}

void CreateHold::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSweep::CreateSweep(Token* repeats, Token* from, Token* to, Token* length) :
    repeats(repeats), from(from), to(to), length(length) {}

void CreateSweep::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateLFO::CreateLFO(Token* repeats, Token* from, Token* to, Token* length) :
    repeats(repeats), from(from), to(to), length(length) {}

void CreateLFO::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
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
    Sine* sine = new Sine(0, 0, 0);

    prepareForVisit(token->volume, &sine->volume);
    prepareForVisit(token->pan, &sine->pan);
    prepareForVisit(token->frequency, &sine->frequency);

    sources.push_back(sine);
}

void ProgramVisitor::visit(CreateSquare* token)
{
    Square* square = new Square(0, 0, 0);

    prepareForVisit(token->volume, &square->volume);
    prepareForVisit(token->pan, &square->pan);
    prepareForVisit(token->frequency, &square->frequency);

    sources.push_back(square);
}

void ProgramVisitor::visit(CreateSaw* token)
{
    Saw* saw = new Saw(0, 0, 0);

    prepareForVisit(token->volume, &saw->volume);
    prepareForVisit(token->pan, &saw->pan);
    prepareForVisit(token->frequency, &saw->frequency);

    sources.push_back(saw);
}

void ProgramVisitor::visit(CreateHold* token)
{
    Hold* hold = new Hold(0, 0);

    prepareForVisit(token->value, &hold->value);
    prepareForVisit(token->length, &hold->length);

    controllerManager->connectParameter(hold, slots.top());

    hold->start();
}

void ProgramVisitor::visit(CreateSweep* token)
{
    Sweep* sweep = new Sweep(0, 0, 0, 0);

    prepareForVisit(token->repeats, &sweep->repeats);
    prepareForVisit(token->from, &sweep->from);
    prepareForVisit(token->to, &sweep->to);
    prepareForVisit(token->length, &sweep->length);

    controllerManager->connectParameter(sweep, slots.top());

    sweep->start();
}

void ProgramVisitor::visit(CreateLFO* token)
{
    LFO* lfo = new LFO(0, 0, 0, 0);

    prepareForVisit(token->repeats, &lfo->repeats);
    prepareForVisit(token->from, &lfo->from);
    prepareForVisit(token->to, &lfo->to);
    prepareForVisit(token->length, &lfo->length);

    controllerManager->connectParameter(lfo, slots.top());

    lfo->start();
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