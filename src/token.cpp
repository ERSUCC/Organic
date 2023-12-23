#include "../include/token.h"

Token::Token(int line, int character) : line(line), character(character) {}

Object* Token::accept(ProgramVisitor* visitor)
{
    return nullptr;
}

Constant::Constant(int line, int character, double value) : Token(line, character), value(value) {}

Object* Constant::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

Name::Name(int line, int character, std::string name) : Token(line, character), name(name) {}

VariableRef::VariableRef(int line, int character, std::string name) : Name(line, character, name) {}

Object* VariableRef::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

Argument::Argument(int line, int character, Name* name, Token* value) : Token(line, character), name(name), value(value) {}

List::List(int line, int character) : Token(line, character) {}

CreateValueCombination::CreateValueCombination(int line, int character, Token* value1, Token* value2) :
    Token(line, character), value1(value1), value2(value2) {}

CreateValueAdd::CreateValueAdd(int line, int character, Token* value1, Token* value2) :
    CreateValueCombination(line, character, value1, value2) {}

Object* CreateValueAdd::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateValueSubtract::CreateValueSubtract(int line, int character, Token* value1, Token* value2) :
    CreateValueCombination(line, character, value1, value2) {}

Object* CreateValueSubtract::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateValueMultiply::CreateValueMultiply(int line, int character, Token* value1, Token* value2) :
    CreateValueCombination(line, character, value1, value2) {}

Object* CreateValueMultiply::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateValueDivide::CreateValueDivide(int line, int character, Token* value1, Token* value2) :
    CreateValueCombination(line, character, value1, value2) {}

Object* CreateValueDivide::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

Instruction::Instruction(int line, int character) : Token(line, character) {}

Assign::Assign(int line, int character, std::string variable, Token* value) :
    Instruction(line, character), variable(variable), value(value) {}

Object* Assign::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateAudioSource::CreateAudioSource(int line, int character, Token* volume, Token* pan, List* effects) :
    Instruction(line, character), volume(volume), pan(pan), effects(effects) {}

CreateOscillator::CreateOscillator(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateAudioSource(line, character, volume, pan, effects), frequency(frequency) {}

CreateSine::CreateSine(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(line, character, volume, pan, frequency, effects) {}

Object* CreateSine::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateSquare::CreateSquare(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(line, character, volume, pan, frequency, effects) {}

Object* CreateSquare::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateSaw::CreateSaw(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(line, character, volume, pan, frequency, effects) {}

Object* CreateSaw::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateTriangle::CreateTriangle(int line, int character, Token* volume, Token* pan, Token* frequency, List* effects) :
    CreateOscillator(line, character, volume, pan, frequency, effects) {}

Object* CreateTriangle::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateHold::CreateHold(int line, int character, Token* value, Token* length) :
    Instruction(line, character), value(value), length(length) {}

Object* CreateHold::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateSweep::CreateSweep(int line, int character, Token* from, Token* to, Token* length) :
    Instruction(line, character), from(from), to(to), length(length) {}

Object* CreateSweep::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateLFO::CreateLFO(int line, int character, Token* from, Token* to, Token* length) :
    Instruction(line, character), from(from), to(to), length(length) {}

Object* CreateLFO::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

GroupOrder::GroupOrder(int line, int character, Sequence::OrderEnum order) :
    Token(line, character), order(order) {}

Object* GroupOrder::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateSequence::CreateSequence(int line, int character, List* controllers, Token* order) :
    Instruction(line, character), controllers(controllers), order(order) {}

Object* CreateSequence::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateRepeat::CreateRepeat(int line, int character, Token* value, Token* repeats) :
    Instruction(line, character), value(value), repeats(repeats) {}

Object* CreateRepeat::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

RandomType::RandomType(int line, int character, Random::TypeEnum type) :
    Token(line, character), type(type) {}

Object* RandomType::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateRandom::CreateRandom(int line, int character, Token* from, Token* to, Token* length, RandomType* type) :
    Instruction(line, character), from(from), to(to), length(length), type(type) {}

Object* CreateRandom::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

CreateEffect::CreateEffect(int line, int character, Token* mix) :
    Instruction(line, character), mix(mix) {}

CreateDelay::CreateDelay(int line, int character, Token* mix, Token* delay, Token* feedback) :
    CreateEffect(line, character, mix), delay(delay), feedback(feedback) {}

Object* CreateDelay::accept(ProgramVisitor* visitor)
{
    return visitor->visit(this);
}

ProgramVisitor::ProgramVisitor(const std::string sourcePath) : sourcePath(sourcePath) {}

Object* ProgramVisitor::visit(Constant* token)
{
    return new Value(token->value);
}

Object* ProgramVisitor::visit(VariableRef* token)
{
    if (!variables.count(token->name))
    {
        Utils::parseError("Variable \"" + token->name + "\" not defined.", sourcePath, token->line, token->character);
    }

    if (currentVariable == token->name)
    {
        Utils::parseError("Variable \"" + token->name + "\" referenced in its own definition.", sourcePath, token->line, token->character);
    }

    return variables[token->name];
}

Object* ProgramVisitor::visit(CreateValueAdd* token)
{
    ValueAdd* add = new ValueAdd();

    add->value1 = (ValueObject*)token->value1->accept(this);
    add->value2 = (ValueObject*)token->value2->accept(this);

    return add;
}

Object* ProgramVisitor::visit(CreateValueSubtract* token)
{
    ValueSubtract* subtract = new ValueSubtract();

    subtract->value1 = (ValueObject*)token->value1->accept(this);
    subtract->value2 = (ValueObject*)token->value2->accept(this);

    return subtract;
}

Object* ProgramVisitor::visit(CreateValueMultiply* token)
{
    ValueMultiply* multiply = new ValueMultiply();

    multiply->value1 = (ValueObject*)token->value1->accept(this);
    multiply->value2 = (ValueObject*)token->value2->accept(this);

    return multiply;
}

Object* ProgramVisitor::visit(CreateValueDivide* token)
{
    ValueDivide* divide = new ValueDivide();

    divide->value1 = (ValueObject*)token->value1->accept(this);
    divide->value2 = (ValueObject*)token->value2->accept(this);

    return divide;
}

Object* ProgramVisitor::visit(Assign* token)
{
    if (!variables.count(token->variable))
    {
        variables.insert(std::make_pair(token->variable, new Variable()));
    }

    currentVariable = token->variable;

    variables[token->variable]->value = (ValueObject*)token->value->accept(this);

    currentVariable = "";

    return variables[token->variable];
}

Object* ProgramVisitor::visit(CreateSine* token)
{
    Sine* sine = new Sine();

    sine->volume = (ValueObject*)token->volume->accept(this);
    sine->pan = (ValueObject*)token->pan->accept(this);
    sine->frequency = (ValueObject*)token->frequency->accept(this);

    for (Token* effect : token->effects->items)
    {
        sine->addEffect((Effect*)effect->accept(this));
    }

    sources.push_back(sine);

    return sine;
}

Object* ProgramVisitor::visit(CreateSquare* token)
{
    Square* square = new Square();

    square->volume = (ValueObject*)token->volume->accept(this);
    square->pan = (ValueObject*)token->pan->accept(this);
    square->frequency = (ValueObject*)token->frequency->accept(this);

    for (Token* effect : token->effects->items)
    {
        square->addEffect((Effect*)effect->accept(this));
    }

    sources.push_back(square);

    return square;
}

Object* ProgramVisitor::visit(CreateSaw* token)
{
    Saw* saw = new Saw();

    saw->volume = (ValueObject*)token->volume->accept(this);
    saw->pan = (ValueObject*)token->pan->accept(this);
    saw->frequency = (ValueObject*)token->frequency->accept(this);

    for (Token* effect : token->effects->items)
    {
        saw->addEffect((Effect*)effect->accept(this));
    }

    sources.push_back(saw);

    return saw;
}

Object* ProgramVisitor::visit(CreateTriangle* token)
{
    Triangle* triangle = new Triangle();

    triangle->volume = (ValueObject*)token->volume->accept(this);
    triangle->pan = (ValueObject*)token->pan->accept(this);
    triangle->frequency = (ValueObject*)token->frequency->accept(this);

    for (Token* effect : token->effects->items)
    {
        triangle->addEffect((Effect*)effect->accept(this));
    }

    sources.push_back(triangle);

    return triangle;
}

Object* ProgramVisitor::visit(CreateHold* token)
{
    Hold* hold = new Hold();

    hold->value = (ValueObject*)token->value->accept(this);
    hold->length = (ValueObject*)token->length->accept(this);

    return hold;
}

Object* ProgramVisitor::visit(CreateSweep* token)
{
    Sweep* sweep = new Sweep();

    sweep->from = (ValueObject*)token->from->accept(this);
    sweep->to = (ValueObject*)token->to->accept(this);
    sweep->length = (ValueObject*)token->length->accept(this);

    return sweep;
}

Object* ProgramVisitor::visit(CreateLFO* token)
{
    LFO* lfo = new LFO();

    lfo->from = (ValueObject*)token->from->accept(this);
    lfo->to = (ValueObject*)token->to->accept(this);
    lfo->length = (ValueObject*)token->length->accept(this);

    return lfo;
}

Object* ProgramVisitor::visit(GroupOrder* token)
{
    return new Sequence::Order(token->order);
}

Object* ProgramVisitor::visit(CreateSequence* token)
{
    Sequence* sequence = new Sequence();

    for (Token* controller : token->controllers->items)
    {
        sequence->controllers.push_back((ValueObject*)controller->accept(this));
    }

    sequence->order = (Sequence::Order*)token->order->accept(this);

    return sequence;
}

Object* ProgramVisitor::visit(CreateRepeat* token)
{
    Repeat* repeat = new Repeat();

    repeat->value = (ValueObject*)token->value->accept(this);
    repeat->repeats = (ValueObject*)token->repeats->accept(this);

    return repeat;
}

Object* ProgramVisitor::visit(RandomType* token)
{
    return new Random::Type(token->type);
}

Object* ProgramVisitor::visit(CreateRandom* token)
{
    Random* random = new Random();

    random->from = (ValueObject*)token->from->accept(this);
    random->to = (ValueObject*)token->to->accept(this);
    random->length = (ValueObject*)token->length->accept(this);
    random->type = (Random::Type*)token->type->accept(this);

    return random;
}

Object* ProgramVisitor::visit(CreateDelay* token)
{
    Delay* delay = new Delay();

    delay->mix = (ValueObject*)token->mix->accept(this);
    delay->delay = (ValueObject*)token->delay->accept(this);
    delay->feedback = (ValueObject*)token->feedback->accept(this);

    return delay;
}

Object* ProgramVisitor::visit(Program* token)
{
    for (Instruction* instruction : token->instructions)
    {
        instruction->accept(this);
    }

    return nullptr;
}
