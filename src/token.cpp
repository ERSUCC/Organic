#include "../include/token.h"

using namespace Parser;

Token::Token(const SourceLocation location) :
    location(location) {}

void Token::resolveTypes(TypeResolver* visitor) {}

Engine::ValueObject* Token::transform(TokenTransformer* visitor) const
{
    return nullptr;
}

const std::string Token::string() const
{
    return location.string();
}

OpenParenthesis::OpenParenthesis(const SourceLocation location) :
    Token(location) {}

CloseParenthesis::CloseParenthesis(const SourceLocation location) :
    Token(location) {}

OpenSquareBracket::OpenSquareBracket(const SourceLocation location) :
    Token(location) {}

CloseSquareBracket::CloseSquareBracket(const SourceLocation location) :
    Token(location) {}

OpenCurlyBracket::OpenCurlyBracket(const SourceLocation location) :
    Token(location) {}

CloseCurlyBracket::CloseCurlyBracket(const SourceLocation location) :
    Token(location) {}

Colon::Colon(const SourceLocation location) :
    Token(location) {}

Comma::Comma(const SourceLocation location) :
    Token(location) {}

Equals::Equals(const SourceLocation location) :
    Token(location) {}

Operator::Operator(const SourceLocation location) :
    Token(location) {}

Add::Add(const SourceLocation location) :
    Operator(location) {}

Subtract::Subtract(const SourceLocation location) :
    Operator(location) {}

Multiply::Multiply(const SourceLocation location) :
    Operator(location) {}

Divide::Divide(const SourceLocation location) :
    Operator(location) {}

Power::Power(const SourceLocation location) :
    Operator(location) {}

DoubleEquals::DoubleEquals(const SourceLocation location) :
    Operator(location) {}

Less::Less(const SourceLocation location) :
    Operator(location) {}

Greater::Greater(const SourceLocation location) :
    Operator(location) {}

LessEqual::LessEqual(const SourceLocation location) :
    Operator(location) {}

GreaterEqual::GreaterEqual(const SourceLocation location) :
    Operator(location) {}

Identifier::Identifier(const SourceLocation location) :
    Token(location) {}

EmptyLambda::EmptyLambda(const SourceLocation location, Token* value) :
    Token(location), value(value) {}

Engine::ValueObject* EmptyLambda::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Value::Value(const SourceLocation location, const double value) :
    Token(location), value(value)
{
    type = new NumberType();
}

Engine::ValueObject* Value::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Constant::Constant(const SourceLocation location, const unsigned char value) :
    Token(location), value(value) {}

Engine::ValueObject* Constant::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

SequenceForward::SequenceForward(const SourceLocation location) :
    Constant(location, Constants::Sequence::Forward)
{
    type = new SequenceOrderType();
}

SequenceBackward::SequenceBackward(const SourceLocation location) :
    Constant(location, Constants::Sequence::Backward)
{
    type = new SequenceOrderType();
}

SequencePingPong::SequencePingPong(const SourceLocation location) :
    Constant(location, Constants::Sequence::PingPong)
{
    type = new SequenceOrderType();
}

SequenceRandom::SequenceRandom(const SourceLocation location) :
    Constant(location, Constants::Sequence::Random)
{
    type = new SequenceOrderType();
}

RandomStep::RandomStep(const SourceLocation location) :
    Constant(location, Constants::Random::Step)
{
    type = new RandomTypeType();
}

RandomLinear::RandomLinear(const SourceLocation location) :
    Constant(location, Constants::Random::Linear)
{
    type = new RandomTypeType();
}

RoundNearest::RoundNearest(const SourceLocation location) :
    Constant(location, Constants::Round::Nearest)
{
    type = new RoundDirectionType();
}

RoundUp::RoundUp(const SourceLocation location) :
    Constant(location, Constants::Round::Up)
{
    type = new RoundDirectionType();
}

RoundDown::RoundDown(const SourceLocation location) :
    Constant(location, Constants::Round::Down)
{
    type = new RoundDirectionType();
}

String::String(const SourceLocation location, const std::string str) :
    Token(location), str(str)
{
    type = new StringType();
}

VariableDef::VariableDef(const SourceLocation location) :
    Identifier(location) {}

VariableRef::VariableRef(const SourceLocation location, VariableDef* definition) :
    Identifier(location), definition(definition) {}

void VariableRef::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* VariableRef::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

InputDef::InputDef(const SourceLocation location, Token* defaultValue) :
    Identifier(location), defaultValue(defaultValue) {}

void InputDef::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

InputRef::InputRef(const SourceLocation location, InputDef* definition) :
    Identifier(location), definition(definition) {}

void InputRef::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* InputRef::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

FunctionDef::FunctionDef(const SourceLocation location, const std::vector<InputDef*>& inputs) :
    Identifier(location), inputs(inputs) {}

FunctionRef::FunctionRef(const SourceLocation location, FunctionDef* definition) :
    Identifier(location), definition(definition) {}

void FunctionRef::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* FunctionRef::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Argument::Argument(const SourceLocation location, const std::string name, Token* value) :
    Token(location), name(name), value(value) {}

ArgumentList::ArgumentList(const SourceLocation location, const std::vector<Argument*>& arguments, const std::string name) :
    Token(location), arguments(arguments), name(name) {}

Token* ArgumentList::get(const std::string name) const
{
    for (size_t i = 0; i < arguments.size(); i++)
    {
        if (arguments[i]->name == name)
        {
            return arguments[i]->value;
        }
    }

    return nullptr;
}

void ArgumentList::addDefault(const std::string name, Token* value)
{
    Argument* argument = new Argument(location, name, value);

    argument->used = true;

    arguments.push_back(argument);
}

void ArgumentList::check() const
{
    for (const Argument* argument : arguments)
    {
        if (!argument->used)
        {
            throw OrganicParseException("Invalid input name \"" + argument->name + "\" for function \"" + name + "\".", argument->location);
        }
    }
}

List::List(const SourceLocation location, const std::vector<Token*> values) :
    Token(location), values(values) {}

void List::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* List::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

ParenthesizedExpression::ParenthesizedExpression(const SourceLocation location, Token* value) :
    Token(location), value(value) {}

void ParenthesizedExpression::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* ParenthesizedExpression::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Assign::Assign(const SourceLocation location, VariableDef* variable, Token* value) :
    Token(location), variable(variable), value(value)
{
    type = new NoneType();
}

void Assign::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Assign::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Call::Call(const SourceLocation location, ArgumentList* arguments) :
    Token(location), arguments(arguments) {}

Time::Time(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Time::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Time::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Hold::Hold(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Hold::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Hold::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

LFO::LFO(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void LFO::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* LFO::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Sweep::Sweep(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Sweep::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Sweep::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Sequence::Sequence(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Sequence::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Sequence::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Repeat::Repeat(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Repeat::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Repeat::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Random::Random(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Random::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Random::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Limit::Limit(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Limit::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Limit::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Trigger::Trigger(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Trigger::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Trigger::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

If::If(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void If::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* If::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

All::All(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new BooleanType();
}

void All::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* All::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Any::Any(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new BooleanType();
}

void Any::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Any::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

None::None(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new BooleanType();
}

void None::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* None::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Min::Min(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Min::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Min::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Max::Max(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Max::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Max::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Round::Round(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Round::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Round::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Absolute::Absolute(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new NumberType();
}

void Absolute::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Absolute::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

AudioSource::AudioSource(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new AudioSourceType();
}

EmptyAudioSource::EmptyAudioSource(const SourceLocation location) :
    AudioSource(location, {}) {}

Engine::ValueObject* EmptyAudioSource::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Sine::Sine(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Sine::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Sine::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Square::Square(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Square::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Square::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Triangle::Triangle(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Triangle::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Triangle::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Saw::Saw(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Saw::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Saw::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Oscillator::Oscillator(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Oscillator::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Oscillator::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Noise::Noise(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Noise::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Noise::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Sample::Sample(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Sample::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Sample::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Granulate::Granulate(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Granulate::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Granulate::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Group::Group(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Group::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Group::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Effect::Effect(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments)
{
    type = new EffectType();
}

EmptyEffect::EmptyEffect(const SourceLocation location) :
    Effect(location, {}) {}

Engine::ValueObject* EmptyEffect::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

EffectGroup::EffectGroup(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void EffectGroup::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* EffectGroup::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Delay::Delay(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void Delay::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Delay::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Comb::Comb(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void Comb::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Comb::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

AllPass::AllPass(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void AllPass::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* AllPass::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

LowPass::LowPass(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void LowPass::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* LowPass::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Reverb::Reverb(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void Reverb::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Reverb::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

CallUser::CallUser(const SourceLocation location, ArgumentList* arguments, FunctionRef* function) :
    Call(location, arguments), function(function) {}

void CallUser::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* CallUser::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

CallAlias::CallAlias(const SourceLocation location, Token* a, Token* b, const std::string op) :
    Call(location, new ArgumentList(location, { new Argument(a->location, "a", a), new Argument(b->location, "b", b) }, op)), op(op) {}

void CallAlias::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

AddAlias::AddAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, "+")
{
    type = new NumberType();
}

Engine::ValueObject* AddAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

SubtractAlias::SubtractAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, "-")
{
    type = new NumberType();
}

Engine::ValueObject* SubtractAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

MultiplyAlias::MultiplyAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, "*")
{
    type = new NumberType();
}

Engine::ValueObject* MultiplyAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

DivideAlias::DivideAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, "/")
{
    type = new NumberType();
}

Engine::ValueObject* DivideAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

PowerAlias::PowerAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, "^")
{
    type = new NumberType();
}

Engine::ValueObject* PowerAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

EqualAlias::EqualAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, "==")
{
    type = new BooleanType();
}

Engine::ValueObject* EqualAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

LessAlias::LessAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, "<")
{
    type = new BooleanType();
}

Engine::ValueObject* LessAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

GreaterAlias::GreaterAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, ">")
{
    type = new BooleanType();
}

Engine::ValueObject* GreaterAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

LessEqualAlias::LessEqualAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, "<=")
{
    type = new BooleanType();
}

Engine::ValueObject* LessEqualAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

GreaterEqualAlias::GreaterEqualAlias(const SourceLocation location, Token* a, Token* b) :
    CallAlias(location, a, b, ">=")
{
    type = new BooleanType();
}

Engine::ValueObject* GreaterEqualAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Define::Define(const SourceLocation location, const std::vector<Token*> instructions, FunctionDef* function) :
    Token(location), instructions(instructions), function(function)
{
    type = new NoneType();
}

void Define::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Define::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Program::Program(const SourceLocation location, const std::vector<Token*> instructions) :
    Token(location), instructions(instructions)
{
    type = new NoneType();
}

void Program::resolveTypes(TypeResolver* visitor)
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Program::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}
