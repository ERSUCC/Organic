#include "../include/token.h"

namespace Parser
{
    Type::Type(const std::string str) :
        str(str) {}

    std::string Type::name() const
    {
        return str;
    }

    bool Type::checkSpecifiedType(const NoneType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const AnyType* expected) const
    {
        return true;
    }

    bool Type::checkSpecifiedType(const SequenceOrderType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const RandomTypeType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const RoundDirectionType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const NumberType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const BooleanType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const StringType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const ListType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const AudioSourceType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const EffectType* expected) const
    {
        return false;
    }

    bool Type::checkSpecifiedType(const LambdaType* expected) const
    {
        return false;
    }

    Token* Type::getDefault(const SourceLocation location)
    {
        return new Value(location, "0", 0);
    }

    NoneType::NoneType() :
        Type("nothing") {}

    bool NoneType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool NoneType::checkSpecifiedType(const NoneType* expected) const
    {
        return true;
    }

    AnyType::AnyType() :
        Type("anything") {}

    bool AnyType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    SequenceOrderType::SequenceOrderType() :
        Type("sequence order constant") {}

    bool SequenceOrderType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool SequenceOrderType::checkSpecifiedType(const SequenceOrderType* expected) const
    {
        return true;
    }

    Token* SequenceOrderType::getDefault(const SourceLocation location)
    {
        return new SequenceForwards(location);
    }

    RandomTypeType::RandomTypeType() :
        Type("random type constant") {}

    bool RandomTypeType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool RandomTypeType::checkSpecifiedType(const RandomTypeType* expected) const
    {
        return true;
    }

    Token* RandomTypeType::getDefault(const SourceLocation location)
    {
        return new RandomStep(location);
    }

    RoundDirectionType::RoundDirectionType() :
        Type("round direction constant") {}

    bool RoundDirectionType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool RoundDirectionType::checkSpecifiedType(const RoundDirectionType* expected) const
    {
        return true;
    }

    Token* RoundDirectionType::getDefault(const SourceLocation location)
    {
        return new RoundNearest(location);
    }

    NumberType::NumberType() :
        Type("number") {}

    bool NumberType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool NumberType::checkSpecifiedType(const NumberType* expected) const
    {
        return true;
    }

    BooleanType::BooleanType() :
        Type("boolean") {}

    bool BooleanType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool BooleanType::checkSpecifiedType(const BooleanType* expected) const
    {
        return true;
    }

    StringType::StringType() :
        Type("string") {}

    bool StringType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool StringType::checkSpecifiedType(const StringType* expected) const
    {
        return true;
    }

    AudioSourceType::AudioSourceType() :
        Type("audio source") {}

    bool AudioSourceType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool AudioSourceType::checkSpecifiedType(const AudioSourceType* expected) const
    {
        return true;
    }

    Token* AudioSourceType::getDefault(const SourceLocation location)
    {
        return new EmptyAudioSource(location);
    }

    EffectType::EffectType() :
        Type("effect") {}

    bool EffectType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool EffectType::checkSpecifiedType(const EffectType* expected) const
    {
        return true;
    }

    Token* EffectType::getDefault(const SourceLocation location)
    {
        return new EmptyEffect(location);
    }

    ListType::ListType(Type* subType) :
        Type("list of " + subType->name()), subType(subType) {}

    bool ListType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool ListType::checkSpecifiedType(const ListType* expected) const
    {
        return expected->subType->checkType(subType);
    }

    Token* ListType::getDefault(const SourceLocation location)
    {
        return new List(location, {});
    }

    LambdaType::LambdaType(const std::unordered_map<std::string, const Type*> inputTypes, const Type* returnType) :
        Type("function " + inputString(inputTypes) + "returning " + returnType->name()), inputTypes(inputTypes), returnType(returnType) {}

    bool LambdaType::checkType(const Type* actual) const
    {
        return actual->checkSpecifiedType(this);
    }

    bool LambdaType::checkSpecifiedType(const LambdaType* expected) const
    {
        if (!expected->returnType->checkType(returnType))
        {
            return false;
        }

        for (const std::pair<std::string, const Type*>& input : expected->inputTypes)
        {
            if (!inputTypes.count(input.first) || !input.second->checkType(inputTypes.at(input.first)))
            {
                return false;
            }
        }

        return true;
    }

    Token* LambdaType::getDefault(const SourceLocation location)
    {
        return new EmptyLambda(location);
    }

    std::string LambdaType::inputString(const std::unordered_map<std::string, const Type*>& inputTypes) const
    {
        if (inputTypes.empty())
        {
            return "with no inputs ";
        }

        std::string str;

        if (inputTypes.size() == 1)
        {
            str += "with input (";
        }

        else
        {
            str += "with inputs (";
        }

        bool first = true;

        for (const std::pair<std::string, const Type*>& input : inputTypes)
        {
            if (first)
            {
                str += input.first + ": " + input.second->name();
            }

            else
            {
                str += ", " + input.first + ": " + input.second->name();
            }

            first = false;
        }

        return str + ") ";
    }

    Token::Token(const SourceLocation location) :
        location(location) {}

    void Token::resolveTypes(TypeResolver* visitor) {}
    void Token::transform(BytecodeTransformer* visitor) const {}

    std::string Token::string() const
    {
        return "";
    }

    BasicToken::BasicToken(const SourceLocation location, const std::string str) :
        Token(location), str(str) {}

    std::string BasicToken::string() const
    {
        return str;
    }

    OpenParenthesis::OpenParenthesis(const SourceLocation location) :
        BasicToken(location, "(") {}

    CloseParenthesis::CloseParenthesis(const SourceLocation location) :
        BasicToken(location, ")") {}

    OpenSquareBracket::OpenSquareBracket(const SourceLocation location) :
        BasicToken(location, "[") {}

    CloseSquareBracket::CloseSquareBracket(const SourceLocation location) :
        BasicToken(location, "]") {}

    OpenCurlyBracket::OpenCurlyBracket(const SourceLocation location) :
        BasicToken(location, "{") {}

    CloseCurlyBracket::CloseCurlyBracket(const SourceLocation location) :
        BasicToken(location, "}") {}

    Colon::Colon(const SourceLocation location) :
        BasicToken(location, ":") {}

    Comma::Comma(const SourceLocation location) :
        BasicToken(location, ",") {}

    Equals::Equals(const SourceLocation location) :
        BasicToken(location, "=") {}

    Operator::Operator(const SourceLocation location, const std::string str) :
        BasicToken(location, str) {}

    Add::Add(const SourceLocation location) :
        Operator(location, "+") {}

    Subtract::Subtract(const SourceLocation location) :
        Operator(location, "-") {}

    Multiply::Multiply(const SourceLocation location) :
        Operator(location, "*") {}

    Divide::Divide(const SourceLocation location) :
        Operator(location, "/") {}

    Power::Power(const SourceLocation location) :
        Operator(location, "^") {}

    DoubleEquals::DoubleEquals(const SourceLocation location) :
        Operator(location, "==") {}

    Less::Less(const SourceLocation location) :
        Operator(location, "<") {}

    Greater::Greater(const SourceLocation location) :
        Operator(location, ">") {}

    LessEqual::LessEqual(const SourceLocation location) :
        Operator(location, "<=") {}

    GreaterEqual::GreaterEqual(const SourceLocation location) :
        Operator(location, ">=") {}

    Identifier::Identifier(const SourceLocation location, const std::string str) :
        BasicToken(location, str) {}

    EmptyLambda::EmptyLambda(const SourceLocation location) :
        Token(location) {}

    void EmptyLambda::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Value::Value(const SourceLocation location, const std::string str, const double value) :
        BasicToken(location, str), value(value)
    {
        type = new NumberType();
    }

    void Value::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Constant::Constant(const SourceLocation location, const std::string str, const unsigned char value) :
        BasicToken(location, str), value(value) {}

    void Constant::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SequenceForwards::SequenceForwards(const SourceLocation location) :
        Constant(location, "sequence-forwards", ::Sequence::OrderEnum::Forwards)
    {
        type = new SequenceOrderType();
    }

    SequenceBackwards::SequenceBackwards(const SourceLocation location) :
        Constant(location, "sequence-backwards", ::Sequence::OrderEnum::Backwards)
    {
        type = new SequenceOrderType();
    }

    SequencePingPong::SequencePingPong(const SourceLocation location) :
        Constant(location, "sequence-ping-pong", ::Sequence::OrderEnum::PingPong)
    {
        type = new SequenceOrderType();
    }

    SequenceRandom::SequenceRandom(const SourceLocation location) :
        Constant(location, "sequence-random", ::Sequence::OrderEnum::Random)
    {
        type = new SequenceOrderType();
    }

    RandomStep::RandomStep(const SourceLocation location) :
        Constant(location, "random-step", ::Random::TypeEnum::Step)
    {
        type = new RandomTypeType();
    }

    RandomLinear::RandomLinear(const SourceLocation location) :
        Constant(location, "random-linear", ::Random::TypeEnum::Linear)
    {
        type = new RandomTypeType();
    }

    RoundNearest::RoundNearest(const SourceLocation location) :
        Constant(location, "round-nearest", ::Round::DirectionEnum::Nearest)
    {
        type = new RoundDirectionType();
    }

    RoundUp::RoundUp(const SourceLocation location) :
        Constant(location, "round-up", ::Round::DirectionEnum::Up)
    {
        type = new RoundDirectionType();
    }

    RoundDown::RoundDown(const SourceLocation location) :
        Constant(location, "round-down", ::Round::DirectionEnum::Down)
    {
        type = new RoundDirectionType();
    }

    String::String(const SourceLocation location, const std::string str) :
        BasicToken(location, "\"" + str + "\"")
    {
        type = new StringType();
    }

    VariableDef::VariableDef(const SourceLocation location, const std::string str) :
        Identifier(location, str) {}

    VariableRef::VariableRef(const SourceLocation location, VariableDef* definition) :
        Identifier(location, definition->str), definition(definition) {}

    void VariableRef::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void VariableRef::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    InputDef::InputDef(const SourceLocation location, const std::string str) :
        Identifier(location, str) {}

    InputRef::InputRef(const SourceLocation location, InputDef* definition) :
        Identifier(location, definition->str), definition(definition) {}

    void InputRef::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void InputRef::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    FunctionDef::FunctionDef(const SourceLocation location, const std::string str, const std::vector<InputDef*>& inputs) :
        Identifier(location, str), inputs(inputs) {}

    FunctionRef::FunctionRef(const SourceLocation location, FunctionDef* definition) :
        Identifier(location, definition->str), definition(definition) {}

    void FunctionRef::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void FunctionRef::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Argument::Argument(const SourceLocation location, const std::string name, Token* value) :
        Token(location), name(name), value(value) {}

    std::string Argument::string() const
    {
        return name + ": " + value->string();
    }

    ArgumentList::ArgumentList(const SourceLocation location, const std::vector<Argument*>& arguments, const std::string name) :
        Token(location), arguments(arguments), name(name) {}

    std::string ArgumentList::string() const
    {
        if (arguments.empty())
        {
            return "";
        }

        std::string result = arguments[0]->string();

        for (unsigned int i = 1; i < arguments.size(); i++)
        {
            result += ", " + arguments[i]->string();
        }

        return result;
    }

    Token* ArgumentList::get(const std::string name) const
    {
        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                return arguments[i]->value;
            }
        }

        return nullptr;
    }

    void ArgumentList::addDefault(const std::string name, Type* type)
    {
        Argument* argument = new Argument(location, name, type->getDefault(location));

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

    void List::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    std::string List::string() const
    {
        std::string result = "[ " + values[0]->string();

        for (unsigned int i = 1; i < values.size(); i++)
        {
            result += ", " + values[i]->string();
        }

        return result + " ]";
    }

    ParenthesizedExpression::ParenthesizedExpression(const SourceLocation location, Token* value) :
        Token(location), value(value) {}

    void ParenthesizedExpression::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void ParenthesizedExpression::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    std::string ParenthesizedExpression::string() const
    {
        return "(" + value->string() + ")";
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

    void Assign::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    std::string Assign::string() const
    {
        return variable->string() + " = " + value->string();
    }

    Call::Call(const SourceLocation location, ArgumentList* arguments) :
        Token(location), arguments(arguments) {}

    std::string Call::string() const
    {
        return arguments->name + '(' + arguments->string() + ')';
    }

    Time::Time(const SourceLocation location, ArgumentList* arguments) :
        Call(location, arguments)
    {
        type = new NumberType();
    }

    void Time::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Time::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Hold::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void LFO::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Sweep::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Sequence::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Repeat::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Random::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Limit::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Trigger::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void If::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void All::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Any::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void None::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Min::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Max::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Round::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    AudioSource::AudioSource(const SourceLocation location, ArgumentList* arguments) :
        Call(location, arguments)
    {
        type = new AudioSourceType();
    }

    EmptyAudioSource::EmptyAudioSource(const SourceLocation location) :
        AudioSource(location, {}) {}

    void EmptyAudioSource::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sine::Sine(const SourceLocation location, ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Sine::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sine::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Square::Square(const SourceLocation location, ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Square::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Square::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Triangle::Triangle(const SourceLocation location, ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Triangle::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Triangle::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Saw::Saw(const SourceLocation location, ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Saw::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Saw::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Oscillator::Oscillator(const SourceLocation location, ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Oscillator::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Oscillator::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Noise::Noise(const SourceLocation location, ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Noise::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Noise::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sample::Sample(const SourceLocation location, ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Sample::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sample::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Play::Play(const SourceLocation location, AudioSource* audioSource) :
        Token(location), audioSource(audioSource)
    {
        type = new AudioSourceType();
    }

    void Play::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Play::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    std::string Play::string() const
    {
        return audioSource->string();
    }

    Effect::Effect(const SourceLocation location, ArgumentList* arguments) :
        Call(location, arguments)
    {
        type = new EffectType();
    }

    EmptyEffect::EmptyEffect(const SourceLocation location) :
        Effect(location, {}) {}

    void EmptyEffect::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Delay::Delay(const SourceLocation location, ArgumentList* arguments) :
        Effect(location, arguments) {}

    void Delay::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Delay::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    CallUser::CallUser(const SourceLocation location, ArgumentList* arguments, FunctionRef* function) :
        Call(location, arguments), function(function) {}

    void CallUser::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void CallUser::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    CallAlias::CallAlias(const SourceLocation location, Token* a, Token* b, const std::string op) :
        Token(location), a(a), b(b), op(op) {}

    void CallAlias::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    std::string CallAlias::string() const
    {
        return a->string() + " " + op + " " + b->string();
    }

    AddAlias::AddAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "+")
    {
        type = new NumberType();
    }

    void AddAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SubtractAlias::SubtractAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "-")
    {
        type = new NumberType();
    }

    void SubtractAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    MultiplyAlias::MultiplyAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "*")
    {
        type = new NumberType();
    }

    void MultiplyAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    DivideAlias::DivideAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "/")
    {
        type = new NumberType();
    }

    void DivideAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    PowerAlias::PowerAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "^")
    {
        type = new NumberType();
    }

    void PowerAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    EqualAlias::EqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "==")
    {
        type = new BooleanType();
    }

    void EqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    LessAlias::LessAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "<")
    {
        type = new BooleanType();
    }

    void LessAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    GreaterAlias::GreaterAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, ">")
    {
        type = new BooleanType();
    }

    void GreaterAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    LessEqualAlias::LessEqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "<=")
    {
        type = new BooleanType();
    }

    void LessEqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    GreaterEqualAlias::GreaterEqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, ">=")
    {
        type = new BooleanType();
    }

    void GreaterEqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
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

    void Define::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    std::string Define::string() const
    {
        std::string result = function->str + '(';

        if (!function->inputs.empty())
        {
            result += function->inputs[0]->string();

            for (unsigned int i = 1; i < function->inputs.size(); i++)
            {
                result += ", " + function->inputs[i]->string();
            }
        }

        result += ") = {";

        if (!instructions.empty())
        {
            result += '\n' + instructions[0]->string();

            for (unsigned int i = 1; i < instructions.size(); i++)
            {
                result += '\n' + instructions[i]->string();
            }
        }

        return result + "\n}";
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

    void Program::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    std::string Program::string() const
    {
        std::string result;

        for (const Token* instruction : instructions)
        {
            result += instruction->string() + '\n';
        }

        return result;
    }

    TypeResolver::TypeResolver(const Path* sourcePath) :
        sourcePath(sourcePath) {}

    void TypeResolver::resolveTypes(VariableRef* token)
    {
        token->type = token->definition->type;
        token->definition->used = true;
    }

    void TypeResolver::resolveTypes(InputRef* token)
    {
        if (!token->definition->type)
        {
            token->definition->type = expectedType;
        }

        token->type = token->definition->type;
        token->definition->used = true;
    }

    void TypeResolver::resolveTypes(FunctionRef* token)
    {
        token->type = token->definition->type;
        token->definition->used = true;
    }

    void TypeResolver::resolveTypes(List* token)
    {
        Type* innerType;

        if (const ListType* listType = dynamic_cast<ListType*>(expectedType))
        {
            innerType = listType->subType;
        }

        else
        {
            innerType = new AnyType();
        }

        for (Token* value : token->values)
        {
            expectedType = innerType;

            value->resolveTypes(this);

            if (!token->values[0]->type->checkType(value->type))
            {
                throw OrganicParseException("All elements in a list must have the same type.", value->location);
            }
        }

        token->type = new ListType(token->values[0]->type);
    }

    void TypeResolver::resolveTypes(ParenthesizedExpression* token)
    {
        token->value->resolveTypes(this);

        token->type = token->value->type;
    }

    void TypeResolver::resolveTypes(Assign* token)
    {
        token->value->resolveTypes(this);

        if (token->value->type->checkSpecifiedType(new NoneType()))
        {
            throw OrganicParseException("Functions that return nothing cannot be assigned to a variable.", token->value->location);
        }

        token->variable->type = token->value->type;
    }

    void TypeResolver::resolveTypes(Time* token)
    {
        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Hold* token)
    {
        resolveArgumentTypes(token->arguments, "length", new NumberType());
        resolveArgumentTypes(token->arguments, "value", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(LFO* token)
    {
        resolveArgumentTypes(token->arguments, "length", new NumberType());
        resolveArgumentTypes(token->arguments, "to", new NumberType());
        resolveArgumentTypes(token->arguments, "from", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Sweep* token)
    {
        resolveArgumentTypes(token->arguments, "length", new NumberType());
        resolveArgumentTypes(token->arguments, "to", new NumberType());
        resolveArgumentTypes(token->arguments, "from", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Sequence* token)
    {
        resolveArgumentTypes(token->arguments, "order", new SequenceOrderType());
        resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Repeat* token)
    {
        resolveArgumentTypes(token->arguments, "repeats", new NumberType());
        resolveArgumentTypes(token->arguments, "value", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Random* token)
    {
        resolveArgumentTypes(token->arguments, "type", new RandomTypeType());
        resolveArgumentTypes(token->arguments, "length", new NumberType());
        resolveArgumentTypes(token->arguments, "to", new NumberType());
        resolveArgumentTypes(token->arguments, "from", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Limit* token)
    {
        resolveArgumentTypes(token->arguments, "max", new NumberType());
        resolveArgumentTypes(token->arguments, "min", new NumberType());
        resolveArgumentTypes(token->arguments, "value", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Trigger* token)
    {
        resolveArgumentTypes(token->arguments, "value", new NumberType());
        resolveArgumentTypes(token->arguments, "condition", new BooleanType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(If* token)
    {
        resolveArgumentTypes(token->arguments, "false", new NumberType());
        resolveArgumentTypes(token->arguments, "true", new NumberType());
        resolveArgumentTypes(token->arguments, "condition", new BooleanType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(All* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Any* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(None* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Min* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Max* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Round* token)
    {
        resolveArgumentTypes(token->arguments, "direction", new RoundDirectionType());
        resolveArgumentTypes(token->arguments, "step", new NumberType());
        resolveArgumentTypes(token->arguments, "value", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Sine* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Square* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Triangle* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Saw* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Oscillator* token)
    {
        resolveArgumentTypes(token->arguments, "waveform", new LambdaType({ { "phase", new NumberType() } }, new NumberType()));
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Noise* token)
    {
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Sample* token)
    {
        resolveArgumentTypes(token->arguments, "file", new StringType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(Play* token)
    {
        token->audioSource->resolveTypes(this);
    }

    void TypeResolver::resolveTypes(Delay* token)
    {
        resolveArgumentTypes(token->arguments, "feedback", new NumberType());
        resolveArgumentTypes(token->arguments, "delay", new NumberType());
        resolveArgumentTypes(token->arguments, "mix", new NumberType());

        token->arguments->check();
    }

    void TypeResolver::resolveTypes(CallUser* token)
    {
        for (const InputDef* input : token->function->definition->inputs)
        {
            if (Type* type = input->type)
            {
                resolveArgumentTypes(token->arguments, input->str, type);
            }

            else
            {
                resolveArgumentTypes(token->arguments, input->str, new AnyType());
            }
        }

        token->arguments->check();

        token->type = token->function->definition->type;
        token->function->definition->used = true;
    }

    void TypeResolver::resolveTypes(CallAlias* token)
    {
        NumberType* number = new NumberType();

        expectedType = number;

        token->a->resolveTypes(this);

        expectedType = number;

        token->b->resolveTypes(this);

        if (!token->a->type->checkSpecifiedType(number))
        {
            throw OrganicParseException("Expected \"" + number->name() + "\", received \"" + token->a->type->name() + "\".", token->a->location);
        }

        if (!token->b->type->checkSpecifiedType(number))
        {
            throw OrganicParseException("Expected \"" + number->name() + "\", received \"" + token->b->type->name() + "\".", token->b->location);
        }
    }

    void TypeResolver::resolveTypes(Define* token)
    {
        for (Token* instruction : token->instructions)
        {
            instruction->resolveTypes(this);
        }

        if (token->instructions.empty())
        {
            Utils::parseWarning("This function contains no instructions.", token->location);

            token->type = new NoneType();
        }

        else
        {
            token->type = token->instructions.back()->type;
        }

        if (!token->type)
        {
            throw OrganicParseException("This function has an ambiguous return type.", token->location);
        }

        std::unordered_map<std::string, const Type*> inputTypes;

        for (const InputDef* input : token->function->inputs)
        {
            inputTypes[input->str] = input->type;
        }

        token->function->type = new LambdaType(inputTypes, token->type);
    }

    void TypeResolver::resolveTypes(Program* token)
    {
        for (Token* instruction : token->instructions)
        {
            instruction->resolveTypes(this);
        }
    }

    void TypeResolver::resolveArgumentTypes(ArgumentList* arguments, const std::string name, Type* expectedType)
    {
        for (unsigned int i = 0; i < arguments->arguments.size(); i++)
        {
            Argument* argument = arguments->arguments[i];

            if (argument->name == name)
            {
                if (argument->used)
                {
                    throw OrganicParseException("Input \"" + argument->name + "\" specified more than once for function \"" + arguments->name + "\".", argument->location);
                }

                argument->used = true;

                this->expectedType = expectedType;

                argument->value->resolveTypes(this);

                const Type* argumentType = argument->value->type;

                if (!expectedType->checkType(argumentType))
                {
                    throw OrganicParseException("Expected \"" + expectedType->name() + "\", received \"" + argumentType->name() + "\".", argument->value->location);
                }

                return;
            }
        }

        arguments->addDefault(name, expectedType);
    }

    BytecodeTransformer::BytecodeTransformer(const Path* sourcePath, std::ofstream& outputStream) :
        sourcePath(sourcePath), outputStream(outputStream) {}

    void BytecodeTransformer::transform(const Value* token)
    {
        addInstruction(new StackPushDouble(token->value));
    }

    void BytecodeTransformer::transform(const Constant* token)
    {
        addInstruction(new StackPushByte(token->value));
    }

    void BytecodeTransformer::transform(const VariableRef* token)
    {
        addInstruction(new GetVariable(token->definition->id));
    }

    void BytecodeTransformer::transform(const InputRef* token)
    {
        addInstruction(new GetVariable(token->definition->id));
    }

    void BytecodeTransformer::transform(const FunctionRef* token)
    {
        std::vector<std::pair<std::string, unsigned char>> inputs;

        for (const InputDef* input : token->definition->inputs)
        {
            inputs.push_back(std::make_pair(input->str, input->id));
        }

        std::sort(inputs.begin(), inputs.end(), std::greater());

        for (const std::pair<std::string, unsigned char>& input : inputs)
        {
            addInstruction(new GetVariable(input.second));
        }

        addInstruction(new StackPushAddress(token->definition->block));
        addInstruction(new StackPushInt(token->definition->inputs.size()));
        addInstruction(new CallNative(BytecodeConstants::LAMBDA, 2));
    }

    void BytecodeTransformer::transform(const EmptyLambda* token)
    {
        addInstruction(new CallNative(BytecodeConstants::EMPTY_LAMBDA, 0));
    }

    void BytecodeTransformer::transform(const List* token)
    {
        for (int i = token->values.size() - 1; i >= 0; i--)
        {
            token->values[i]->transform(this);
        }

        addInstruction(new StackPushInt(token->values.size()));
        addInstruction(new CallNative(BytecodeConstants::LIST, 1));
    }

    void BytecodeTransformer::transform(const ParenthesizedExpression* token)
    {
        token->value->transform(this);
    }

    void BytecodeTransformer::transform(const Assign* token)
    {
        if (token->variable->used)
        {
            token->value->transform(this);

            token->variable->id = nextIdentifierId++;

            addInstruction(new SetVariable(token->variable->id));
        }

        else if (token->variable->location.path == sourcePath)
        {
            Utils::parseWarning("Unused variable \"" + token->variable->str + "\".", token->variable->location);
        }
    }

    void BytecodeTransformer::transform(const Time* token)
    {
        addInstruction(new CallNative(BytecodeConstants::TIME, 0));
    }

    void BytecodeTransformer::transform(const Hold* token)
    {
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "value");

        addInstruction(new CallNative(BytecodeConstants::HOLD, 2));
    }

    void BytecodeTransformer::transform(const LFO* token)
    {
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        addInstruction(new CallNative(BytecodeConstants::LFO, 3));
    }

    void BytecodeTransformer::transform(const Sweep* token)
    {
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        addInstruction(new CallNative(BytecodeConstants::SWEEP, 3));
    }

    void BytecodeTransformer::transform(const Sequence* token)
    {
        transformArgument(token->arguments, "order");
        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::SEQUENCE, 2));
    }

    void BytecodeTransformer::transform(const Repeat* token)
    {
        transformArgument(token->arguments, "repeats");
        transformArgument(token->arguments, "value");

        addInstruction(new CallNative(BytecodeConstants::REPEAT, 2));
    }

    void BytecodeTransformer::transform(const Random* token)
    {
        transformArgument(token->arguments, "type");
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        addInstruction(new CallNative(BytecodeConstants::RANDOM, 4));
    }

    void BytecodeTransformer::transform(const Limit* token)
    {
        transformArgument(token->arguments, "max");
        transformArgument(token->arguments, "min");
        transformArgument(token->arguments, "value");

        addInstruction(new CallNative(BytecodeConstants::LIMIT, 3));
    }

    void BytecodeTransformer::transform(const Trigger* token)
    {
        transformArgument(token->arguments, "value");
        transformArgument(token->arguments, "condition");

        addInstruction(new CallNative(BytecodeConstants::TRIGGER, 2));
    }

    void BytecodeTransformer::transform(const If* token)
    {
        transformArgument(token->arguments, "false");
        transformArgument(token->arguments, "true");
        transformArgument(token->arguments, "condition");

        addInstruction(new CallNative(BytecodeConstants::IF, 3));
    }

    void BytecodeTransformer::transform(const All* token)
    {
        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::ALL, 1));
    }

    void BytecodeTransformer::transform(const Any* token)
    {
        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::ANY, 1));
    }

    void BytecodeTransformer::transform(const None* token)
    {
        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::NONE, 1));
    }

    void BytecodeTransformer::transform(const Min* token)
    {
        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::MIN, 1));
    }

    void BytecodeTransformer::transform(const Max* token)
    {
        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::MAX, 1));
    }

    void BytecodeTransformer::transform(const Round* token)
    {
        transformArgument(token->arguments, "direction");
        transformArgument(token->arguments, "step");
        transformArgument(token->arguments, "value");

        addInstruction(new CallNative(BytecodeConstants::ROUND, 3));
    }

    void BytecodeTransformer::transform(const EmptyAudioSource* token)
    {
        addInstruction(new CallNative(BytecodeConstants::EMPTY_AUDIO_SOURCE, 0));
    }

    void BytecodeTransformer::transform(const Sine* token)
    {
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::SINE, 4));
    }

    void BytecodeTransformer::transform(const Square* token)
    {
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::SQUARE, 4));
    }

    void BytecodeTransformer::transform(const Triangle* token)
    {
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::TRIANGLE, 4));
    }

    void BytecodeTransformer::transform(const Saw* token)
    {
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::SAW, 4));
    }

    void BytecodeTransformer::transform(const Oscillator* token)
    {
        transformArgument(token->arguments, "waveform");
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::OSCILLATOR, 5));
    }

    void BytecodeTransformer::transform(const Noise* token)
    {
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::NOISE, 3));
    }

    void BytecodeTransformer::transform(const Sample* token)
    {
        const String* str = dynamic_cast<const String*>(token->arguments->get("file"));

        const Path* path = Path::beside(Path::formatPath(str->str), sourcePath);

        if (!path->exists())
        {
            throw OrganicIncludeException("Audio file \"" + path->string() + "\" does not exist.", str->location);
        }

        if (!path->isFile())
        {
            throw OrganicIncludeException("\"" + path->string() + "\" is not a file.", str->location);
        }

        if (!resources.count(path))
        {
            resources[path] = resources.size();

            resolver->addResourceBlock(new ResourceBlock(path));
        }

        addInstruction(new StackPushResource(resources[path]));

        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::SAMPLE, 4));
    }

    void BytecodeTransformer::transform(const Play* token)
    {
        token->audioSource->transform(this);

        addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
    }

    void BytecodeTransformer::transform(const EmptyEffect* token)
    {
        addInstruction(new CallNative(BytecodeConstants::EMPTY_EFFECT, 0));
    }

    void BytecodeTransformer::transform(const Delay* token)
    {
        transformArgument(token->arguments, "feedback");
        transformArgument(token->arguments, "delay");
        transformArgument(token->arguments, "mix");

        addInstruction(new CallNative(BytecodeConstants::DELAY, 3));
    }

    void BytecodeTransformer::transform(const CallUser* token)
    {
        for (const InputDef* input : token->function->definition->inputs)
        {
            transformArgument(token->arguments, input->str);

            addInstruction(new SetVariable(input->id));
        }

        addInstruction(new ::CallUser(token->function->definition->block, token->function->definition->inputs.size()));
    }

    void BytecodeTransformer::transform(const AddAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::ADD, 2));
    }

    void BytecodeTransformer::transform(const SubtractAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::SUBTRACT, 2));
    }

    void BytecodeTransformer::transform(const MultiplyAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::MULTIPLY, 2));
    }

    void BytecodeTransformer::transform(const DivideAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::DIVIDE, 2));
    }

    void BytecodeTransformer::transform(const PowerAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::POWER, 2));
    }

    void BytecodeTransformer::transform(const EqualAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::EQUAL, 2));
    }

    void BytecodeTransformer::transform(const LessAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::LESS, 2));
    }

    void BytecodeTransformer::transform(const GreaterAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::GREATER, 2));
    }

    void BytecodeTransformer::transform(const LessEqualAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::LESS_EQUAL, 2));
    }

    void BytecodeTransformer::transform(const GreaterEqualAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::GREATER_EQUAL, 2));
    }

    void BytecodeTransformer::transform(const Define* token)
    {
        if (token->function->used)
        {
            for (InputDef* input : token->function->inputs)
            {
                if (input->used)
                {
                    input->id = nextIdentifierId++;
                }

                else
                {
                    Utils::parseWarning("Unused input \"" + input->str + "\".", input->location);
                }
            }

            blocks.push(token->function->block);

            for (const Token* instruction : token->instructions)
            {
                instruction->transform(this);
            }

            blocks.pop();

            resolver->addInstructionBlock(token->function->block);
        }

        else if (token->function->location.path == sourcePath)
        {
            Utils::parseWarning("Unused function \"" + token->function->str + "\".", token->function->location);
        }
    }

    void BytecodeTransformer::transform(const Program* token)
    {
        InstructionBlock* block = new InstructionBlock();

        blocks.push(block);

        resolver->addInstructionBlock(block);

        for (const Token* instruction : token->instructions)
        {
            instruction->transform(this);
        }

        blocks.pop();

        resolver->output(outputStream, nextIdentifierId);
    }

    void BytecodeTransformer::transformArgument(const ArgumentList* arguments, const std::string name)
    {
        for (unsigned int i = 0; i < arguments->arguments.size(); i++)
        {
            if (arguments->arguments[i]->name == name)
            {
                arguments->arguments[i]->value->transform(this);

                return;
            }
        }
    }

    void BytecodeTransformer::addInstruction(const BytecodeInstruction* instruction)
    {
        blocks.top()->addInstruction(instruction);
    }
}
