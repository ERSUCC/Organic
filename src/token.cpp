#include "../include/token.h"

Token::Token(int line, int character) : line(line), character(character) {}

Constant::Constant(int line, int character, double value) : Token(line, character), value(value) {}

void Constant::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Name::Name(int line, int character, std::string name) : Token(line, character), name(name) {}

VariableRef::VariableRef(int line, int character, std::string name) : Name(line, character, name) {}

void VariableRef::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Argument::Argument(int line, int character, Name* name, Token* value) : Token(line, character), name(name), value(value) {}

List::List(int line, int character) : Token(line, character) {}

CreateValueCombination::CreateValueCombination(int line, int character, Token* value1, Token* value2) :
    Token(line, character), value1(value1), value2(value2) {}

CreateValueAdd::CreateValueAdd(int line, int character, Token* value1, Token* value2) :
    CreateValueCombination(line, character, value1, value2) {}

void CreateValueAdd::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateValueSubtract::CreateValueSubtract(int line, int character, Token* value1, Token* value2) :
    CreateValueCombination(line, character, value1, value2) {}

void CreateValueSubtract::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateValueMultiply::CreateValueMultiply(int line, int character, Token* value1, Token* value2) :
    CreateValueCombination(line, character, value1, value2) {}

void CreateValueMultiply::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateValueDivide::CreateValueDivide(int line, int character, Token* value1, Token* value2) :
    CreateValueCombination(line, character, value1, value2) {}

void CreateValueDivide::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

Instruction::Instruction(int line, int character) : Token(line, character) {}

Assign::Assign(int line, int character, std::string variable, Token* value) :
    Instruction(line, character), variable(variable), value(value) {}

void Assign::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateAudioSource::CreateAudioSource(int line, int character, Token* volume, Token* pan, List* effects) :
    Instruction(line, character), volume(volume), pan(pan), effects(effects) {}

CreateOscillator::CreateOscillator(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateAudioSource(line, character, volume, pan, effects), frequency(frequency) {}

CreateSine::CreateSine(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(line, character, volume, pan, frequency, effects) {}

void CreateSine::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSquare::CreateSquare(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(line, character, volume, pan, frequency, effects) {}

void CreateSquare::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSaw::CreateSaw(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(line, character, volume, pan, frequency, effects) {}

void CreateSaw::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateTriangle::CreateTriangle(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(line, character, volume, pan, frequency, effects) {}

void CreateTriangle::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateHold::CreateHold(int line, int character, Token* value, Token* length) :
    Instruction(line, character), value(value), length(length) {}

void CreateHold::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSweep::CreateSweep(int line, int character, Token* repeats, Token* from, Token* to, Token* length) :
    Instruction(line, character), repeats(repeats), from(from), to(to), length(length) {}

void CreateSweep::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateLFO::CreateLFO(int line, int character, Token* repeats, Token* from, Token* to, Token* length) :
    Instruction(line, character), repeats(repeats), from(from), to(to), length(length) {}

void CreateLFO::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

GroupOrder::GroupOrder(int line, int character, Sequence::OrderEnum order) :
    Token(line, character), order(order) {}

void GroupOrder::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateSequence::CreateSequence(int line, int character, Token* repeats, List* controllers, Token* order) :
    Instruction(line, character), repeats(repeats), controllers(controllers), order(order) {}

void CreateSequence::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

RandomType::RandomType(int line, int character, Random::TypeEnum type) :
    Token(line, character), type(type) {}

void RandomType::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateRandom::CreateRandom(int line, int character, Token* repeats, Token* from, Token* to, Token* length, RandomType* type) :
    Instruction(line, character), repeats(repeats), from(from), to(to), length(length), type(type) {}

void CreateRandom::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

CreateEffect::CreateEffect(int line, int character, Token* mix) :
    Instruction(line, character), mix(mix) {}

CreateDelay::CreateDelay(int line, int character, Token* mix, Token* delay, Token* feedback) :
    CreateEffect(line, character, mix), delay(delay), feedback(feedback) {}

void CreateDelay::accept(ProgramVisitor* visitor)
{
    visitor->visit(this);
}

void ProgramVisitor::visit(Constant* token)
{
    setLastSlot(new Value(token->value));
}

void ProgramVisitor::visit(VariableRef* token)
{
    if (!variables.count(token->name))
    {
        Utils::error("Variable '" + token->name + "' not defined.");
    }

    setLastSlot(variables[token->name]);
}

void ProgramVisitor::visit(CreateValueAdd* token)
{
    ValueAdd* add = new ValueAdd();

    visitWithSlot(token->value1, (Object**)&add->value1);
    visitWithSlot(token->value2, (Object**)&add->value2);

    setLastSlot(add);
}

void ProgramVisitor::visit(CreateValueSubtract* token)
{
    ValueSubtract* subtract = new ValueSubtract();

    visitWithSlot(token->value1, (Object**)&subtract->value1);
    visitWithSlot(token->value2, (Object**)&subtract->value2);

    setLastSlot(subtract);
}

void ProgramVisitor::visit(CreateValueMultiply* token)
{
    ValueMultiply* multiply = new ValueMultiply();

    visitWithSlot(token->value1, (Object**)&multiply->value1);
    visitWithSlot(token->value2, (Object**)&multiply->value2);

    setLastSlot(multiply);
}

void ProgramVisitor::visit(CreateValueDivide* token)
{
    ValueDivide* divide = new ValueDivide();

    visitWithSlot(token->value1, (Object**)&divide->value1);
    visitWithSlot(token->value2, (Object**)&divide->value2);

    setLastSlot(divide);
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

    setLastSlot(hold);
}

void ProgramVisitor::visit(CreateSweep* token)
{
    Sweep* sweep = new Sweep();

    visitWithSlot(token->repeats, (Object**)&sweep->repeats);
    visitWithSlot(token->from, (Object**)&sweep->from);
    visitWithSlot(token->to, (Object**)&sweep->to);
    visitWithSlot(token->length, (Object**)&sweep->length);

    setLastSlot(sweep);
}

void ProgramVisitor::visit(CreateLFO* token)
{
    LFO* lfo = new LFO();

    visitWithSlot(token->repeats, (Object**)&lfo->repeats);
    visitWithSlot(token->from, (Object**)&lfo->from);
    visitWithSlot(token->to, (Object**)&lfo->to);
    visitWithSlot(token->length, (Object**)&lfo->length);

    setLastSlot(lfo);
}

void ProgramVisitor::visit(GroupOrder* token)
{
    setLastSlot(new Sequence::Order(token->order));
}

void ProgramVisitor::visit(CreateSequence* token)
{
    Sequence* sequence = new Sequence();

    visitWithSlot(token->repeats, (Object**)&sequence->repeats);

    for (Token* controller : token->controllers->items)
    {
        ValueObject* result;

        visitWithSlot(controller, (Object**)&result);

        sequence->controllers.push_back(result);
    }

    visitWithSlot(token->order, (Object**)&sequence->order);

    setLastSlot(sequence);
}

void ProgramVisitor::visit(RandomType* token)
{
    setLastSlot(new Random::Type(token->type));
}

void ProgramVisitor::visit(CreateRandom* token)
{
    Random* random = new Random();

    visitWithSlot(token->repeats, (Object**)&random->repeats);
    visitWithSlot(token->from, (Object**)&random->from);
    visitWithSlot(token->to, (Object**)&random->to);
    visitWithSlot(token->length, (Object**)&random->length);
    visitWithSlot(token->type, (Object**)&random->type);

    setLastSlot(random);
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

void ProgramVisitor::setLastSlot(Object* value)
{
    if (!slots.empty())
    {
        *slots.top() = value;
    }
}
