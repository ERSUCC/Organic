#include "../include/token.h"

using namespace Parser;

Token::Token(const SourceLocation location, const Type* type) :
    location(location), staticType(type) {}

Token::~Token() {}

const SharedType Token::type() const
{
    return staticType;
}

const std::string Token::string() const
{
    return location.string();
}

bool Token::eof() const
{
    return false;
}

void Token::resolveTypes(const TypeResolver* visitor) const {}

Engine::ValueObject* Token::transform(TokenTransformer* visitor) const
{
    return nullptr;
}

Eof::Eof(const SourceLocation location) :
    Token(location) {}

bool Eof::eof() const
{
    return true;
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

EmptyLambda::EmptyLambda(const SourceLocation location, const Token* value) :
    Token(location), value(value) {}

EmptyLambda::~EmptyLambda()
{
    delete value;
}

Engine::ValueObject* EmptyLambda::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Value::Value(const SourceLocation location, const double value) :
    Token(location, new NumberType()), value(value) {}

Engine::ValueObject* Value::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Constant::Constant(const SourceLocation location, const Type* type, const unsigned char value) :
    Token(location, type), value(value) {}

Engine::ValueObject* Constant::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

String::String(const SourceLocation location, const std::string str) :
    Token(location, new StringType()), str(str) {}

VariableDef::VariableDef(const SourceLocation location, const Token* value) :
    Identifier(location), value(value) {}

VariableDef::~VariableDef()
{
    delete value;
}

void VariableDef::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* VariableDef::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

VariableRef::VariableRef(const SourceLocation location, const VariableDef* definition) :
    Identifier(location), definition(definition) {}

const SharedType VariableRef::type() const
{
    return definition->value->type();
}

Engine::ValueObject* VariableRef::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

InputDef::InputDef(const SourceLocation location, const SharedToken defaultValue) :
    Identifier(location), defaultValue(defaultValue) {}

const SharedType InputDef::type() const
{
    return defaultValue->type();
}

void InputDef::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

InputRef::InputRef(const SourceLocation location, const InputDef* definition) :
    Identifier(location), definition(definition) {}

const SharedType InputRef::type() const
{
    return definition->type();
}

Engine::ValueObject* InputRef::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

FunctionDef::FunctionDef(const SourceLocation location, const std::vector<const InputDef*>& inputs, const Program* program) :
    Identifier(location), inputs(inputs), program(program) {}

FunctionDef::~FunctionDef()
{
    for (const InputDef* input : inputs)
    {
        delete input;
    }

    delete program;
}

const SharedType FunctionDef::returnType() const
{
    return program->instructions.back()->type();
}

void FunctionDef::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

FunctionRef::FunctionRef(const SourceLocation location, const FunctionDef* definition) :
    Identifier(location), definition(definition) {}

const SharedType FunctionRef::type() const
{
    std::unordered_map<std::string, const SharedType> inputTypes;

    for (const InputDef* input : definition->inputs)
    {
        inputTypes.insert(std::make_pair(input->string(), input->type()));
    }

    return SharedType(new LambdaType(inputTypes, definition->returnType()));
}

Engine::ValueObject* FunctionRef::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Argument::Argument(const SourceLocation location, const std::string name, const SharedToken value) :
    Token(location), name(name), value(value) {}

ArgumentList::ArgumentList(const SourceLocation location, const std::vector<const Argument*>& arguments, const std::string name) :
    Token(location), arguments(arguments), name(name) {}

ArgumentList::~ArgumentList()
{
    for (const Argument* argument : arguments)
    {
        delete argument;
    }
}

const Argument* ArgumentList::findArgument(const std::string name)
{
    for (const Argument* argument : arguments)
    {
        if (argument->name == name)
        {
            used.insert(argument);

            return argument;
        }
    }

    return nullptr;
}

void ArgumentList::addDefault(const std::string name, const SharedToken value)
{
    const Argument* argument = new Argument(location, name, value);

    arguments.push_back(argument);

    used.insert(argument);
}

void ArgumentList::check() const
{
    for (const Argument* argument : arguments)
    {
        if (!used.count(argument))
        {
            throw OrganicParseException("Invalid input name \"" + argument->name + "\" for function \"" + name + "\".", argument->location);
        }
    }
}

List::List(const SourceLocation location, const std::vector<const Token*>& values) :
    Token(location), values(values) {}

List::~List()
{
    for (const Token* value : values)
    {
        delete value;
    }
}

const SharedType List::type() const
{
    return SharedType(new ListType(values[0]->type()));
}

void List::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* List::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

ParenthesizedExpression::ParenthesizedExpression(const SourceLocation location, const Token* value) :
    Token(location), value(value) {}

ParenthesizedExpression::~ParenthesizedExpression()
{
    delete value;
}

const SharedType ParenthesizedExpression::type() const
{
    return value->type();
}

void ParenthesizedExpression::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* ParenthesizedExpression::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Call::Call(const SourceLocation location, ArgumentList* arguments, const Type* type) :
    Token(location, type), arguments(arguments) {}

Call::~Call()
{
    delete arguments;
}

Time::Time(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Time::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Time::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Hold::Hold(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Hold::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Hold::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

LFO::LFO(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void LFO::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* LFO::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Sweep::Sweep(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Sweep::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Sweep::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Sequence::Sequence(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Sequence::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Sequence::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Repeat::Repeat(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Repeat::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Repeat::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Random::Random(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Random::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Random::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Limit::Limit(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Limit::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Limit::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Trigger::Trigger(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Trigger::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Trigger::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

If::If(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void If::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* If::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

All::All(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new BooleanType()) {}

void All::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* All::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Any::Any(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new BooleanType()) {}

void Any::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Any::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

None::None(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new BooleanType()) {}

void None::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* None::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Min::Min(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Min::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Min::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Max::Max(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Max::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Max::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Round::Round(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Round::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Round::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Absolute::Absolute(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new NumberType()) {}

void Absolute::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Absolute::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

AudioSource::AudioSource(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new AudioSourceType()) {}

EmptyAudioSource::EmptyAudioSource(const SourceLocation location) :
    AudioSource(location, new ArgumentList(location, {}, "")) {}

Engine::ValueObject* EmptyAudioSource::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Sine::Sine(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Sine::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Sine::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Square::Square(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Square::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Square::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Triangle::Triangle(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Triangle::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Triangle::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Saw::Saw(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Saw::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Saw::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Oscillator::Oscillator(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Oscillator::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Oscillator::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Noise::Noise(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Noise::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Noise::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Sample::Sample(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Sample::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Sample::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Granulate::Granulate(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Granulate::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Granulate::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Group::Group(const SourceLocation location, ArgumentList* arguments) :
    AudioSource(location, arguments) {}

void Group::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Group::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Effect::Effect(const SourceLocation location, ArgumentList* arguments) :
    Call(location, arguments, new EffectType()) {}

EmptyEffect::EmptyEffect(const SourceLocation location) :
    Effect(location, new ArgumentList(location, {}, "")) {}

Engine::ValueObject* EmptyEffect::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

EffectGroup::EffectGroup(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void EffectGroup::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* EffectGroup::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Delay::Delay(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void Delay::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Delay::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Comb::Comb(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void Comb::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Comb::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

AllPass::AllPass(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void AllPass::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* AllPass::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

LowPass::LowPass(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void LowPass::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* LowPass::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Reverb::Reverb(const SourceLocation location, ArgumentList* arguments) :
    Effect(location, arguments) {}

void Reverb::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Reverb::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

CallUser::CallUser(const SourceLocation location, ArgumentList* arguments, const FunctionDef* function) :
    Call(location, arguments), function(function) {}

const SharedType CallUser::type() const
{
    return function->returnType();
}

void CallUser::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* CallUser::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

CallAlias::CallAlias(const SourceLocation location, const Token* a, const Token* b, const std::string op, const Type* type) :
    Call(location, new ArgumentList(location, { new Argument(a->location, "a", SharedToken(a)), new Argument(b->location, "b", SharedToken(b)) }, op), type), op(op) {}

void CallAlias::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

AddAlias::AddAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, "+", new NumberType()) {}

Engine::ValueObject* AddAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

SubtractAlias::SubtractAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, "-", new NumberType()) {}

Engine::ValueObject* SubtractAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

MultiplyAlias::MultiplyAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, "*", new NumberType()) {}

Engine::ValueObject* MultiplyAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

DivideAlias::DivideAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, "/", new NumberType()) {}

Engine::ValueObject* DivideAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

PowerAlias::PowerAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, "^", new NumberType()) {}

Engine::ValueObject* PowerAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

EqualAlias::EqualAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, "==", new BooleanType()) {}

Engine::ValueObject* EqualAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

LessAlias::LessAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, "<", new BooleanType()) {}

Engine::ValueObject* LessAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

GreaterAlias::GreaterAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, ">", new BooleanType()) {}

Engine::ValueObject* GreaterAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

LessEqualAlias::LessEqualAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, "<=", new BooleanType()) {}

Engine::ValueObject* LessEqualAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

GreaterEqualAlias::GreaterEqualAlias(const SourceLocation location, const Token* a, const Token* b) :
    CallAlias(location, a, b, ">=", new BooleanType()) {}

Engine::ValueObject* GreaterEqualAlias::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}

Program::Program(const SourceLocation location, const std::vector<const Token*>& instructions) :
    Token(location), instructions(instructions) {}

Program::~Program()
{
    for (const Token* instruction : instructions)
    {
        delete instruction;
    }
}

void Program::resolveTypes(const TypeResolver* visitor) const
{
    visitor->resolveTypes(this);
}

Engine::ValueObject* Program::transform(TokenTransformer* visitor) const
{
    return visitor->transform(this);
}
