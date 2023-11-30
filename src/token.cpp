#include "../include/token.h"

Constant::Constant(double value) : value(value) {}

void Constant::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Name::Name(std::string name) : name(name) {}

VariableRef::VariableRef(std::string name) : Name(name) {}

void VariableRef::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Argument::Argument(Name* name, Token* value) : name(name), value(value) {}

GroupOrder::GroupOrder(ControllerGroup::OrderEnum order) : order(order) {}

void GroupOrder::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateValueCombination::CreateValueCombination(Token* value1, Token* value2) : value1(value1), value2(value2) {}

CreateValueAdd::CreateValueAdd(Token* value1, Token* value2) : CreateValueCombination(value1, value2) {}

void CreateValueAdd::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateValueSubtract::CreateValueSubtract(Token* value1, Token* value2) : CreateValueCombination(value1, value2) {}

void CreateValueSubtract::accept(ProgramVisitor* visitor)
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

void ProgramVisitor::visit(Constant* token)
{
    if (!slots.empty())
    {
        *slots.top() = new Value(token->value);
    }
}

void ProgramVisitor::visit(VariableRef* token)
{
    if (!variables.count(token->name))
    {
        Utils::error("Variable '" + token->name + "' not defined.");
    }

    *slots.top() = variables[token->name];
}

void ProgramVisitor::visit(GroupOrder* token)
{
    *slots.top() = new ControllerGroup::Order(token->order);
}

void ProgramVisitor::visit(CreateValueAdd* token)
{
    ValueAdd* add = new ValueAdd();

    visitWithSlot(token->value1, (Object**)&add->value1);
    visitWithSlot(token->value2, (Object**)&add->value2);

    *slots.top() = add;
}

void ProgramVisitor::visit(CreateValueSubtract* token)
{
    ValueSubtract* subtract = new ValueSubtract();

    visitWithSlot(token->value1, (Object**)&subtract->value1);
    visitWithSlot(token->value2, (Object**)&subtract->value2);

    *slots.top() = subtract;
}

void ProgramVisitor::visit(Assign* token)
{
    if (!variables.count(token->variable))
    {
        variables.insert(std::make_pair(token->variable, new Variable()));
    }

    visitWithSlot(token->value, (Object**)&variables[token->variable]->value);
}

void ProgramVisitor::visit(CreateSine* token)
{
    Sine* sine = new Sine();

    visitWithSlot(token->volume, (Object**)&sine->volume);
    visitWithSlot(token->pan, (Object**)&sine->pan);
    visitWithSlot(token->frequency, (Object**)&sine->frequency);

    for (Token* effect : token->effects->items)
    {
        visitWithSlot(effect, (Object**)&sine);
    }

    sources.push_back(sine);
}

void ProgramVisitor::visit(CreateSquare* token)
{
    Square* square = new Square();

    visitWithSlot(token->volume, (Object**)&square->volume);
    visitWithSlot(token->pan, (Object**)&square->pan);
    visitWithSlot(token->frequency, (Object**)&square->frequency);

    for (Token* effect : token->effects->items)
    {
        visitWithSlot(effect, (Object**)&square);
    }

    sources.push_back(square);
}

void ProgramVisitor::visit(CreateSaw* token)
{
    Saw* saw = new Saw();

    visitWithSlot(token->volume, (Object**)&saw->volume);
    visitWithSlot(token->pan, (Object**)&saw->pan);
    visitWithSlot(token->frequency, (Object**)&saw->frequency);

    for (Token* effect : token->effects->items)
    {
        visitWithSlot(effect, (Object**)&saw);
    }

    sources.push_back(saw);
}

void ProgramVisitor::visit(CreateTriangle* token)
{
    Triangle* triangle = new Triangle();

    visitWithSlot(token->volume, (Object**)&triangle->volume);
    visitWithSlot(token->pan, (Object**)&triangle->pan);
    visitWithSlot(token->frequency, (Object**)&triangle->frequency);

    for (Token* effect : token->effects->items)
    {
        visitWithSlot(effect, (Object**)&triangle);
    }

    sources.push_back(triangle);
}

void ProgramVisitor::visit(CreateHold* token)
{
    Hold* hold = new Hold();

    visitWithSlot(token->value, (Object**)&hold->value);
    visitWithSlot(token->length, (Object**)&hold->length);

    *slots.top() = hold;
}

void ProgramVisitor::visit(CreateSweep* token)
{
    Sweep* sweep = new Sweep();

    visitWithSlot(token->repeats, (Object**)&sweep->repeats);
    visitWithSlot(token->from, (Object**)&sweep->from);
    visitWithSlot(token->to, (Object**)&sweep->to);
    visitWithSlot(token->length, (Object**)&sweep->length);

    *slots.top() = sweep;
}

void ProgramVisitor::visit(CreateLFO* token)
{
    LFO* lfo = new LFO();

    visitWithSlot(token->repeats, (Object**)&lfo->repeats);
    visitWithSlot(token->from, (Object**)&lfo->from);
    visitWithSlot(token->to, (Object**)&lfo->to);
    visitWithSlot(token->length, (Object**)&lfo->length);

    *slots.top() = lfo;
}

void ProgramVisitor::visit(CreateControllerGroup* token)
{
    ControllerGroup* group = new ControllerGroup();

    visitWithSlot(token->repeats, (Object**)&group->repeats);

    for (Token* controller : token->controllers->items)
    {
        ValueObject* result;

        visitWithSlot(controller, (Object**)&result);

        group->controllers.push_back(result);
    }

    visitWithSlot(token->order, (Object**)&group->order);

    *slots.top() = group;
}

void ProgramVisitor::visit(CreateDelay* token)
{
    Delay* delay = new Delay();

    visitWithSlot(token->mix, (Object**)&delay->mix);
    visitWithSlot(token->delay, (Object**)&delay->delay);
    visitWithSlot(token->feedback, (Object**)&delay->feedback);

    ((AudioSource*)*slots.top())->addEffect(delay);
}

void ProgramVisitor::visit(Program* token)
{
    for (Instruction* instruction : token->instructions)
    {
        instruction->accept(this);
    }
}

void ProgramVisitor::visitWithSlot(Token* token, Object** slot)
{
    slots.push(slot);

    token->accept(this);

    slots.pop();
}