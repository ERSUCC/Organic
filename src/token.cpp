#include "../include/token.h"

Constant::Constant(double value) : value(value) {}

void Constant::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Name::Name(std::string name) : name(name) {}

Variable::Variable(std::string name) : Name(name) {}

void Variable::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Argument::Argument(Name* name, Token* value) : name(name), value(value) {}

Assign::Assign(std::string variable, Token* value) : variable(variable), value(value) {}

void Assign::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

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
    if (!slots.empty())
    {
        *slots.top() = token->value;
    }
}

void ProgramVisitor::visit(Variable* token)
{
    if (!variables.count(token->name))
    {
        Utils::error("Variable '" + token->name + "' not defined.");
    }

    variables[token->name]->accept(this);
}

void ProgramVisitor::visit(Assign* token)
{
    variables.insert(std::make_pair(token->variable, token->value));
}

void ProgramVisitor::visit(CreateSine* token)
{
    Sine* sine = new Sine(0, 0, 0);

    visitWithParameter(token->volume, &sine->volume);
    visitWithParameter(token->pan, &sine->pan);
    visitWithParameter(token->frequency, &sine->frequency);

    sources.push_back(sine);
}

void ProgramVisitor::visit(CreateSquare* token)
{
    Square* square = new Square(0, 0, 0);

    visitWithParameter(token->volume, &square->volume);
    visitWithParameter(token->pan, &square->pan);
    visitWithParameter(token->frequency, &square->frequency);

    sources.push_back(square);
}

void ProgramVisitor::visit(CreateSaw* token)
{
    Saw* saw = new Saw(0, 0, 0);

    visitWithParameter(token->volume, &saw->volume);
    visitWithParameter(token->pan, &saw->pan);
    visitWithParameter(token->frequency, &saw->frequency);

    sources.push_back(saw);
}

void ProgramVisitor::visit(CreateHold* token)
{
    Hold* hold = new Hold(0, 0);

    visitWithParameter(token->value, &hold->value);
    visitWithParameter(token->length, &hold->length);

    controllerManager->connectParameter(hold, slots.top());

    hold->start();
}

void ProgramVisitor::visit(CreateSweep* token)
{
    Sweep* sweep = new Sweep(0, 0, 0, 0);

    visitWithParameter(token->repeats, &sweep->repeats);
    visitWithParameter(token->from, &sweep->from);
    visitWithParameter(token->to, &sweep->to);
    visitWithParameter(token->length, &sweep->length);

    if (!slots.empty())
    {
        controllerManager->connectParameter(sweep, slots.top());
    }

    sweep->start();
}

void ProgramVisitor::visit(CreateLFO* token)
{
    LFO* lfo = new LFO(0, 0, 0, 0);

    visitWithParameter(token->repeats, &lfo->repeats);
    visitWithParameter(token->from, &lfo->from);
    visitWithParameter(token->to, &lfo->to);
    visitWithParameter(token->length, &lfo->length);

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

void ProgramVisitor::visitWithParameter(Token* token, Parameter* slot)
{
    slots.push(slot);

    token->accept(this);

    slots.pop();
}