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

GroupOrder::GroupOrder(ControllerGroup::OrderEnum order) : order(order) {}

void GroupOrder::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Assign::Assign(std::string variable, Token* value) : variable(variable), value(value) {}

void Assign::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateAudioSource::CreateAudioSource(Token* volume, Token* pan, List* effects) :
    volume(volume), pan(pan), effects(effects) {}

CreateOscillator::CreateOscillator(Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateAudioSource(volume, pan, effects), frequency(frequency) {}

CreateSine::CreateSine(Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(volume, pan, frequency, effects) {}

void CreateSine::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSquare::CreateSquare(Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(volume, pan, frequency, effects) {}

void CreateSquare::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSaw::CreateSaw(Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(volume, pan, frequency, effects) {}

void CreateSaw::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateTriangle::CreateTriangle(Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(volume, pan, frequency, effects) {}

void CreateTriangle::accept(ProgramVisitor* visitor)
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

CreateControllerGroup::CreateControllerGroup(Token* repeats, List* controllers, Token* order) :
    repeats(repeats), controllers(controllers), order(order) {}

void CreateControllerGroup::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateEffect::CreateEffect(Token* mix) : mix(mix) {}

CreateDelay::CreateDelay(Token* mix, Token* delay, Token* feedback) :
    CreateEffect(mix), delay(delay), feedback(feedback) {}

void CreateDelay::accept(ProgramVisitor* visitor)
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
        *(Parameter*)slots.top() = token->value;
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

void ProgramVisitor::visit(GroupOrder* token)
{
    *(ControllerGroup::Order*)slots.top() = token->order;
}

void ProgramVisitor::visit(Assign* token)
{
    variables.insert(std::make_pair(token->variable, token->value));
}

void ProgramVisitor::visit(CreateSine* token)
{
    Sine* sine = new Sine(0, 0, 0);

    visitWithSlot(token->volume, &sine->volume);
    visitWithSlot(token->pan, &sine->pan);
    visitWithSlot(token->frequency, &sine->frequency);

    for (Token* effect : token->effects->items)
    {
        visitWithSlot(effect, sine);
    }

    sources.push_back(sine);
}

void ProgramVisitor::visit(CreateSquare* token)
{
    Square* square = new Square(0, 0, 0);

    visitWithSlot(token->volume, &square->volume);
    visitWithSlot(token->pan, &square->pan);
    visitWithSlot(token->frequency, &square->frequency);

    for (Token* effect : token->effects->items)
    {
        visitWithSlot(effect, square);
    }

    sources.push_back(square);
}

void ProgramVisitor::visit(CreateSaw* token)
{
    Saw* saw = new Saw(0, 0, 0);

    visitWithSlot(token->volume, &saw->volume);
    visitWithSlot(token->pan, &saw->pan);
    visitWithSlot(token->frequency, &saw->frequency);

    for (Token* effect : token->effects->items)
    {
        visitWithSlot(effect, saw);
    }

    sources.push_back(saw);
}

void ProgramVisitor::visit(CreateTriangle* token)
{
    Triangle* triangle = new Triangle(0, 0, 0);

    visitWithSlot(token->volume, &triangle->volume);
    visitWithSlot(token->pan, &triangle->pan);
    visitWithSlot(token->frequency, &triangle->frequency);

    for (Token* effect : token->effects->items)
    {
        visitWithSlot(effect, triangle);
    }

    sources.push_back(triangle);
}

void ProgramVisitor::visit(CreateHold* token)
{
    Hold* hold = new Hold(0, 0);

    visitWithSlot(token->value, &hold->value);
    visitWithSlot(token->length, &hold->length);

    ControllerGroup* group = dynamic_cast<ControllerGroup*>(slots.top());

    if (group)
    {
        group->controllers.push_back(hold);
    }

    else
    {
        controllerManager->connectParameter(hold, (Parameter*)slots.top());

        hold->start();
    }
}

void ProgramVisitor::visit(CreateSweep* token)
{
    Sweep* sweep = new Sweep(0, 0, 0, 0);

    visitWithSlot(token->repeats, &sweep->repeats);
    visitWithSlot(token->from, &sweep->from);
    visitWithSlot(token->to, &sweep->to);
    visitWithSlot(token->length, &sweep->length);

    ControllerGroup* group = dynamic_cast<ControllerGroup*>(slots.top());

    if (group)
    {
        group->controllers.push_back(sweep);
    }

    else
    {
        controllerManager->connectParameter(sweep, (Parameter*)slots.top());

        sweep->start();
    }
}

void ProgramVisitor::visit(CreateLFO* token)
{
    LFO* lfo = new LFO(0, 0, 0, 0);

    visitWithSlot(token->repeats, &lfo->repeats);
    visitWithSlot(token->from, &lfo->from);
    visitWithSlot(token->to, &lfo->to);
    visitWithSlot(token->length, &lfo->length);

    ControllerGroup* group = dynamic_cast<ControllerGroup*>(slots.top());

    if (group)
    {
        group->controllers.push_back(lfo);
    }

    else
    {
        controllerManager->connectParameter(lfo, (Parameter*)slots.top());

        lfo->start();
    }
}

void ProgramVisitor::visit(CreateControllerGroup* token)
{
    ControllerGroup* group = new ControllerGroup(0, {}, ControllerGroup::OrderEnum::Forwards);

    visitWithSlot(token->repeats, &group->repeats);

    for (Token* controller : token->controllers->items)
    {
        visitWithSlot(controller, group);
    }

    visitWithSlot(token->order, &group->order);

    ControllerGroup* group2 = dynamic_cast<ControllerGroup*>(slots.top());

    if (group2)
    {
        group2->controllers.push_back(group);
    }

    else
    {
        controllerManager->connectParameter(group, (Parameter*)slots.top());

        group->start();
    }
}

void ProgramVisitor::visit(CreateDelay* token)
{
    Delay* delay = new Delay(0, 0, 0);

    visitWithSlot(token->mix, &delay->mix);
    visitWithSlot(token->delay, &delay->delay);
    visitWithSlot(token->feedback, &delay->feedback);

    ((AudioSource*)slots.top())->addEffect(delay);
}

void ProgramVisitor::visit(Program* token)
{
    for (Instruction* instruction : token->instructions)
    {
        instruction->accept(this);
    }
}

void ProgramVisitor::visitWithSlot(Token* token, Object* slot)
{
    slots.push(slot);

    token->accept(this);

    slots.pop();
}