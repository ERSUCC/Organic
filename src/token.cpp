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

Argument::Argument(Name* name, Token* value) : name(name), value(value) {}

std::string Argument::toString()
{
    return name->toString() + ": " + value->toString();
}

CreateSine::CreateSine(Token* volume, Token* pan, Token* frequency) :
    volume(volume), pan(pan), frequency(frequency) {}

std::string CreateSine::toString()
{
    return "sine(" + volume->toString() + ", " + pan->toString() + ", " + frequency->toString() + ")";
}

void CreateSine::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSquare::CreateSquare(Token* volume, Token* pan, Token* frequency) :
    volume(volume), pan(pan), frequency(frequency) {}

std::string CreateSquare::toString()
{
    return "square(" + volume->toString() + ", " + pan->toString() + ", " + frequency->toString() + ")";
}

void CreateSquare::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSaw::CreateSaw(Token* volume, Token* pan, Token* frequency) :
    volume(volume), pan(pan), frequency(frequency) {}

std::string CreateSaw::toString()
{
    return "saw(" + volume->toString() + ", " + pan->toString() + ", " + frequency->toString() + ")";
}

void CreateSaw::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSweep::CreateSweep(Token* repeats, Token* floor, Token* ceiling, Token* length) :
    repeats(repeats), floor(floor), ceiling(ceiling), length(length) {}

std::string CreateSweep::toString()
{
    return "sweep(" + repeats->toString() + "," + floor->toString() + "," + ceiling->toString() + ", " + length->toString() + ")";
}

void CreateSweep::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateLFO::CreateLFO(Token* repeats, Token* floor, Token* ceiling, Token* rate) :
    repeats(repeats), floor(floor), ceiling(ceiling), rate(rate) {}

std::string CreateLFO::toString()
{
    return "lfo(" + repeats->toString() + "," + floor->toString() + "," + ceiling->toString() + ", " + rate->toString() + ")";
}

void CreateLFO::accept(ProgramVisitor* visitor)
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

void ProgramVisitor::visit(CreateSweep* token)
{
    Sweep* sweep = new Sweep(0, 0, 0, 0);

    prepareForVisit(token->repeats, &sweep->repeats);
    prepareForVisit(token->floor, &sweep->first);
    prepareForVisit(token->ceiling, &sweep->second);
    prepareForVisit(token->length, &sweep->length);

    controllerManager->connectParameter(sweep, slots.top());

    sweep->start();
}

void ProgramVisitor::visit(CreateLFO* token)
{
    LFO* lfo = new LFO(0, 0, 0, 0);

    prepareForVisit(token->repeats, &lfo->repeats);
    prepareForVisit(token->floor, &lfo->floor);
    prepareForVisit(token->ceiling, &lfo->ceiling);
    prepareForVisit(token->rate, &lfo->rate);

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