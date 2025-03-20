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

    void Identifier::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Identifier::transform(BytecodeTransformer* visitor) const
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

    SequenceForwards::SequenceForwards(const SourceLocation location) :
        BasicToken(location, "sequence-forwards")
    {
        type = new SequenceOrderType();
    }

    void SequenceForwards::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SequenceBackwards::SequenceBackwards(const SourceLocation location) :
        BasicToken(location, "sequence-backwards")
    {
        type = new SequenceOrderType();
    }

    void SequenceBackwards::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SequencePingPong::SequencePingPong(const SourceLocation location) :
        BasicToken(location, "sequence-ping-pong")
    {
        type = new SequenceOrderType();
    }

    void SequencePingPong::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SequenceRandom::SequenceRandom(const SourceLocation location) :
        BasicToken(location, "sequence-random")
    {
        type = new SequenceOrderType();
    }

    void SequenceRandom::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    RandomStep::RandomStep(const SourceLocation location) :
        BasicToken(location, "random-step")
    {
        type = new RandomTypeType();
    }

    void RandomStep::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    RandomLinear::RandomLinear(const SourceLocation location) :
        BasicToken(location, "random-linear")
    {
        type = new RandomTypeType();
    }

    void RandomLinear::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    RoundNearest::RoundNearest(const SourceLocation location) :
        BasicToken(location, "random-linear")
    {
        type = new RoundDirectionType();
    }

    void RoundNearest::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    RoundUp::RoundUp(const SourceLocation location) :
        BasicToken(location, "random-linear")
    {
        type = new RoundDirectionType();
    }

    void RoundUp::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    RoundDown::RoundDown(const SourceLocation location) :
        BasicToken(location, "random-linear")
    {
        type = new RoundDirectionType();
    }

    void RoundDown::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Pi::Pi(const SourceLocation location) :
        BasicToken(location, "pi")
    {
        type = new NumberType();
    }

    void Pi::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    E::E(const SourceLocation location) :
        BasicToken(location, "e")
    {
        type = new NumberType();
    }

    void E::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    String::String(const SourceLocation location, const std::string str) :
        BasicToken(location, str)
    {
        type = new StringType();
    }

    Argument::Argument(const SourceLocation location, const std::string name, Token* value) :
        Token(location), name(name), value(value) {}

    std::string Argument::string() const
    {
        return name + ": " + value->string();
    }

    ArgumentList::ArgumentList(const std::vector<Argument*>& arguments, const std::string name) :
        arguments(arguments), name(name) {}

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

    Assign::Assign(const SourceLocation location, Identifier* variable, Token* value) :
        Token(location), variable(variable), value(value) {}

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

    Call::Call(const SourceLocation location, const ArgumentList* arguments) :
        Token(location), arguments(arguments) {}

    std::string Call::string() const
    {
        std::string result = arguments->name + '(';

        if (!arguments->arguments.empty())
        {
            result += arguments->arguments[0]->string();

            for (unsigned int i = 1; i < arguments->arguments.size(); i++)
            {
                result += ", " + arguments->arguments[i]->string();
            }
        }

        return result + ')';
    }

    Time::Time(const SourceLocation location, const ArgumentList* arguments) :
        Call(location, arguments)
    {
        type = new NumberType();
    }

    void Time::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Hold::Hold(const SourceLocation location, const ArgumentList* arguments) :
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

    LFO::LFO(const SourceLocation location, const ArgumentList* arguments) :
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

    Sweep::Sweep(const SourceLocation location, const ArgumentList* arguments) :
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

    Sequence::Sequence(const SourceLocation location, const ArgumentList* arguments) :
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

    Repeat::Repeat(const SourceLocation location, const ArgumentList* arguments) :
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

    Random::Random(const SourceLocation location, const ArgumentList* arguments) :
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

    Limit::Limit(const SourceLocation location, const ArgumentList* arguments) :
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

    Trigger::Trigger(const SourceLocation location, const ArgumentList* arguments) :
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

    If::If(const SourceLocation location, const ArgumentList* arguments) :
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

    All::All(const SourceLocation location, const ArgumentList* arguments) :
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

    Any::Any(const SourceLocation location, const ArgumentList* arguments) :
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

    None::None(const SourceLocation location, const ArgumentList* arguments) :
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

    Min::Min(const SourceLocation location, const ArgumentList* arguments) :
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

    Max::Max(const SourceLocation location, const ArgumentList* arguments) :
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

    Round::Round(const SourceLocation location, const ArgumentList* arguments) :
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

    AudioSource::AudioSource(const SourceLocation location, const ArgumentList* arguments) :
        Call(location, arguments)
    {
        type = new AudioSourceType();
    }

    Sine::Sine(const SourceLocation location, const ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Sine::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sine::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Square::Square(const SourceLocation location, const ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Square::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Square::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Triangle::Triangle(const SourceLocation location, const ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Triangle::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Triangle::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Saw::Saw(const SourceLocation location, const ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Saw::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Saw::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Oscillator::Oscillator(const SourceLocation location, const ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Oscillator::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Oscillator::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Noise::Noise(const SourceLocation location, const ArgumentList* arguments) :
        AudioSource(location, arguments) {}

    void Noise::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Noise::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sample::Sample(const SourceLocation location, const ArgumentList* arguments) :
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

    Delay::Delay(const SourceLocation location, const ArgumentList* arguments) :
        Call(location, arguments)
    {
        type = new EffectType();
    }

    void Delay::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Delay::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Perform::Perform(const SourceLocation location, const ArgumentList* arguments) :
        Call(location, arguments)
    {
        type = new NoneType();
    }

    void Perform::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Perform::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Include::Include(const SourceLocation location, const ArgumentList* arguments) :
        Call(location, arguments)
    {
        type = new NoneType();
    }

    void Include::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Include::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    CallUser::CallUser(const SourceLocation location, const ArgumentList* arguments) :
        Call(location, arguments) {}

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

    Define::Define(const SourceLocation location, const std::string name, const std::vector<Identifier*> inputs, const std::vector<Token*> instructions) :
        Token(location), name(name), inputs(inputs), instructions(instructions)
    {
        type = new NoneType();

        block = new InstructionBlock();
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
        std::string result = name + '(';

        if (!inputs.empty())
        {
            result += inputs[0]->string();

            for (unsigned int i = 1; i < inputs.size(); i++)
            {
                result += ", " + inputs[i]->string();
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

    Scope::Scope(const std::string name, const std::vector<Identifier*>& inputs) :
        name(name)
    {
        for (Identifier* input : inputs)
        {
            this->inputs[input->str] = input;
        }
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

    TypeResolver::TypeResolver(const Path* sourcePath, ParserInterface* parser) :
        sourcePath(sourcePath), parser(parser) {}

    void TypeResolver::resolveTypes(Identifier* token)
    {
        if (Identifier* input = getInput(token->str))
        {
            if (!input->type)
            {
                input->type = expectedType;
            }

            token->type = input->type;
            token->id = input->id;
        }

        else if (const Identifier* variable = getVariable(token->str))
        {
            token->type = variable->type;
            token->id = variable->id;
        }

        else if (const Define* function = getFunction(token->str))
        {
            std::unordered_map<std::string, const Type*> inputTypes;

            for (const Identifier* input : function->inputs)
            {
                if (input->type)
                {
                    inputTypes[input->str] = input->type;
                }

                else
                {
                    inputTypes[input->str] = new AnyType();
                }
            }

            token->type = new LambdaType(inputTypes, function->type);
            token->id = function->id;
        }

        else
        {
            throw OrganicParseException("Unrecognized variable name \"" + token->str + "\".", token->location);
        }
    }

    void TypeResolver::resolveTypes(List* token)
    {
        if (!expectedType->checkSpecifiedType(new ListType(new AnyType())))
        {
            return;
        }

        Type* innerType = static_cast<ListType*>(expectedType)->subType;

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

        if (getInput(token->variable->str))
        {
            throw OrganicParseException("Function inputs cannot be redefined.", token->location);
        }

        if (getVariable(token->variable->str))
        {
            throw OrganicParseException("Variables cannot be redefined.", token->location);
        }

        if (getFunction(token->variable->str))
        {
            throw OrganicParseException("A function already exists with the name \"" + token->variable->str + "\".", token->location);
        }

        token->variable->type = token->value->type;
        token->variable->id = nextIdentifierId++;

        addVariable(token->variable);
    }

    void TypeResolver::resolveTypes(Hold* token)
    {
        resolveArgumentTypes(token->arguments, "length", new NumberType());
        resolveArgumentTypes(token->arguments, "value", new NumberType());
    }

    void TypeResolver::resolveTypes(LFO* token)
    {
        resolveArgumentTypes(token->arguments, "length", new NumberType());
        resolveArgumentTypes(token->arguments, "to", new NumberType());
        resolveArgumentTypes(token->arguments, "from", new NumberType());
    }

    void TypeResolver::resolveTypes(Sweep* token)
    {
        resolveArgumentTypes(token->arguments, "length", new NumberType());
        resolveArgumentTypes(token->arguments, "to", new NumberType());
        resolveArgumentTypes(token->arguments, "from", new NumberType());
    }

    void TypeResolver::resolveTypes(Sequence* token)
    {
        resolveArgumentTypes(token->arguments, "order", new SequenceOrderType());
        resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));
    }

    void TypeResolver::resolveTypes(Repeat* token)
    {
        resolveArgumentTypes(token->arguments, "repeats", new NumberType());
        resolveArgumentTypes(token->arguments, "value", new NumberType());
    }

    void TypeResolver::resolveTypes(Random* token)
    {
        resolveArgumentTypes(token->arguments, "type", new RandomTypeType());
        resolveArgumentTypes(token->arguments, "length", new NumberType());
        resolveArgumentTypes(token->arguments, "to", new NumberType());
        resolveArgumentTypes(token->arguments, "from", new NumberType());
    }

    void TypeResolver::resolveTypes(Limit* token)
    {
        resolveArgumentTypes(token->arguments, "max", new NumberType());
        resolveArgumentTypes(token->arguments, "min", new NumberType());
        resolveArgumentTypes(token->arguments, "value", new NumberType());
    }

    void TypeResolver::resolveTypes(Trigger* token)
    {
        resolveArgumentTypes(token->arguments, "value", new NumberType());
        resolveArgumentTypes(token->arguments, "condition", new BooleanType());
    }

    void TypeResolver::resolveTypes(If* token)
    {
        resolveArgumentTypes(token->arguments, "false", new NumberType());
        resolveArgumentTypes(token->arguments, "true", new NumberType());
        resolveArgumentTypes(token->arguments, "condition", new BooleanType());
    }

    void TypeResolver::resolveTypes(All* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));
    }

    void TypeResolver::resolveTypes(Any* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));
    }

    void TypeResolver::resolveTypes(None* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));
    }

    void TypeResolver::resolveTypes(Min* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));
    }

    void TypeResolver::resolveTypes(Max* token)
    {
        resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));
    }

    void TypeResolver::resolveTypes(Round* token)
    {
        resolveArgumentTypes(token->arguments, "direction", new RoundDirectionType());
        resolveArgumentTypes(token->arguments, "step", new NumberType());
        resolveArgumentTypes(token->arguments, "value", new NumberType());
    }

    void TypeResolver::resolveTypes(Sine* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());
    }

    void TypeResolver::resolveTypes(Square* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());
    }

    void TypeResolver::resolveTypes(Triangle* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());
    }

    void TypeResolver::resolveTypes(Saw* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());
    }

    void TypeResolver::resolveTypes(Oscillator* token)
    {
        resolveArgumentTypes(token->arguments, "waveform", new LambdaType({ { "phase", new NumberType() } }, new NumberType()));
        resolveArgumentTypes(token->arguments, "frequency", new NumberType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());
    }

    void TypeResolver::resolveTypes(Noise* token)
    {
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());
    }

    void TypeResolver::resolveTypes(Sample* token)
    {
        resolveArgumentTypes(token->arguments, "file", new StringType());
        resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
        resolveArgumentTypes(token->arguments, "pan", new NumberType());
        resolveArgumentTypes(token->arguments, "volume", new NumberType());
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
    }

    void TypeResolver::resolveTypes(Perform* token)
    {
        resolveArgumentTypes(token->arguments, "interval", new NumberType());
        resolveArgumentTypes(token->arguments, "repeats", new NumberType());
        resolveArgumentTypes(token->arguments, "delay", new NumberType());
        resolveArgumentTypes(token->arguments, "function", new AnyType());
    }

    void TypeResolver::resolveTypes(Include* token)
    {
        resolveArgumentTypes(token->arguments, "file", new StringType());

        const String* str = dynamic_cast<const String*>(token->arguments->get("file"));

        if (!str || str->str.empty())
        {
            Utils::includeWarning("This include does not specify a source file, it will have no effect.", token->location);

            return;
        }

        const Path* includePath = Path::beside(str->str, sourcePath);

        if (!includePath->exists())
        {
            throw OrganicIncludeException("Source file \"" + includePath->string() + "\" does not exist.", str->location);
        }

        if (!includePath->isFile())
        {
            throw OrganicIncludeException("\"" + includePath->string() + "\" is not a file.", str->location);
        }

        if (scopes.back()->includedPaths.count(includePath))
        {
            Utils::includeWarning("Source file \"" + includePath->string() + "\" has already been included, this include will be ignored.", str->location);
        }

        else
        {
            token->program = parser->parse(includePath);

            scopes.back()->includedPaths.insert(includePath);

            for (Token* instruction : token->program->instructions)
            {
                instruction->resolveTypes(this);
            }
        }
    }

    void TypeResolver::resolveTypes(CallUser* token)
    {
        if (Define* function = getFunction(token->arguments->name))
        {
            for (const Identifier* input : function->inputs)
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

            token->function = function;
            token->type = function->type;
        }

        else if (checkRecursive(token->arguments->name))
        {
            throw OrganicParseException("Calling a function in its own definition is not allowed.", token->location);
        }

        else
        {
            throw OrganicParseException("Unknown function name \"" + token->arguments->name + "\".", token->location);
        }
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
        if (getInput(token->name))
        {
            throw OrganicParseException("An input already exists with the name \"" + token->name + "\".", token->location);
        }

        if (getVariable(token->name))
        {
            throw OrganicParseException("A variable already exists with the name \"" + token->name + "\".", token->location);
        }

        if (getFunction(token->name))
        {
            throw OrganicParseException("A function already exists with the name \"" + token->name + "\".", token->location);
        }

        if (checkRecursive(token->name))
        {
            throw OrganicParseException("Redefining a function in its own definition is not allowed.", token->location);
        }

        for (Identifier* input : token->inputs)
        {
            input->id = nextIdentifierId++;
        }

        scopes.push_back(new Scope(token->name, token->inputs));

        for (Token* instruction : token->instructions)
        {
            instruction->resolveTypes(this);
        }

        checkUses();

        scopes.pop_back();

        addFunction(token);

        if (token->instructions.empty())
        {
            Utils::parseWarning("This function contains no instructions.", token->location);

            token->type = new NoneType();
        }

        else
        {
            token->type = token->instructions.back()->type;
        }

        token->id = nextIdentifierId++;
    }

    void TypeResolver::resolveTypes(Program* token)
    {
        scopes.push_back(new Scope("", {}));

        scopes.back()->includedPaths.insert(token->location.path);

        for (Token* instruction : token->instructions)
        {
            instruction->resolveTypes(this);
        }

        checkUses();

        scopes.pop_back();
    }

    void TypeResolver::resolveArgumentTypes(const ArgumentList* arguments, const std::string name, Type* expectedType)
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
    }

    Identifier* TypeResolver::getVariable(const std::string variable)
    {
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            if (scopes[i]->variables.count(variable))
            {
                scopes[i]->variables[variable]->used = true;

                return scopes[i]->variables[variable];
            }
        }

        return nullptr;
    }

    void TypeResolver::addVariable(Identifier* variable)
    {
        scopes.back()->variables[variable->str] = variable;
    }

    Identifier* TypeResolver::getInput(const std::string input)
    {
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            if (scopes[i]->inputs.count(input))
            {
                scopes[i]->inputs[input]->used = true;

                return scopes[i]->inputs[input];
            }
        }

        return nullptr;
    }

    Define* TypeResolver::getFunction(const std::string function)
    {
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            if (scopes[i]->functions.count(function))
            {
                scopes[i]->functions[function]->used = true;

                return scopes[i]->functions[function];
            }
        }

        return nullptr;
    }

    void TypeResolver::addFunction(Define* function)
    {
        scopes.back()->functions[function->name] = function;
    }

    bool TypeResolver::checkRecursive(const std::string function) const
    {
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            if (scopes[i]->name == function)
            {
                return true;
            }
        }

        return false;
    }

    void TypeResolver::checkUses() const
    {
        for (const std::pair<std::string, Identifier*>& pair : scopes.back()->variables)
        {
            if (!pair.second->used && pair.second->location.path == sourcePath)
            {
                Utils::parseWarning("Unused variable \"" + pair.first + "\".", pair.second->location);
            }
        }

        for (const std::pair<std::string, Identifier*>& pair : scopes.back()->inputs)
        {
            if (!pair.second->used && pair.second->location.path == sourcePath)
            {
                Utils::parseWarning("Unused input \"" + pair.first + "\".", pair.second->location);
            }
        }

        for (const std::pair<std::string, Define*>& pair : scopes.back()->functions)
        {
            if (!pair.second->used && pair.second->location.path == sourcePath)
            {
                Utils::parseWarning("Unused function \"" + pair.first + "\".", pair.second->location);
            }
        }
    }

    BytecodeTransformer::BytecodeTransformer(const Path* sourcePath, std::ofstream& outputStream) :
        sourcePath(sourcePath), outputStream(outputStream)
    {
        utils = Utils::get();
    }

    void BytecodeTransformer::transform(const Value* token)
    {
        addInstruction(new StackPushDouble(token->value));
    }

    void BytecodeTransformer::transform(const SequenceForwards* token)
    {
        addInstruction(new StackPushByte(::Sequence::OrderEnum::Forwards));
    }

    void BytecodeTransformer::transform(const SequenceBackwards* token)
    {
        addInstruction(new StackPushByte(::Sequence::OrderEnum::Backwards));
    }

    void BytecodeTransformer::transform(const SequencePingPong* token)
    {
        addInstruction(new StackPushByte(::Sequence::OrderEnum::PingPong));
    }

    void BytecodeTransformer::transform(const SequenceRandom* token)
    {
        addInstruction(new StackPushByte(::Sequence::OrderEnum::Random));
    }

    void BytecodeTransformer::transform(const RandomStep* token)
    {
        addInstruction(new StackPushByte(::Random::TypeEnum::Step));
    }

    void BytecodeTransformer::transform(const RandomLinear* token)
    {
        addInstruction(new StackPushByte(::Random::TypeEnum::Linear));
    }

    void BytecodeTransformer::transform(const RoundNearest* token)
    {
        addInstruction(new StackPushByte(::Round::DirectionEnum::Nearest));
    }

    void BytecodeTransformer::transform(const RoundUp* token)
    {
        addInstruction(new StackPushByte(::Round::DirectionEnum::Up));
    }

    void BytecodeTransformer::transform(const RoundDown* token)
    {
        addInstruction(new StackPushByte(::Round::DirectionEnum::Down));
    }

    void BytecodeTransformer::transform(const Pi* token)
    {
        addInstruction(new StackPushDouble(utils->pi));
    }

    void BytecodeTransformer::transform(const E* token)
    {
        addInstruction(new StackPushDouble(utils->e));
    }

    void BytecodeTransformer::transform(const Identifier* token)
    {
        if (token->id >= nextIdentifierId)
        {
            nextIdentifierId = token->id + 1;
        }

        addInstruction(new GetVariable(token->id));
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
        token->value->transform(this);

        if (token->variable->id >= nextIdentifierId)
        {
            nextIdentifierId = token->variable->id + 1;
        }

        addInstruction(new SetVariable(token->variable->id));
    }

    void BytecodeTransformer::transform(const Time* token)
    {
        token->arguments->check();

        addInstruction(new CallNative(BytecodeConstants::TIME, 0));
    }

    void BytecodeTransformer::transform(const Hold* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "value");

        addInstruction(new CallNative(BytecodeConstants::HOLD, 2));
    }

    void BytecodeTransformer::transform(const LFO* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        addInstruction(new CallNative(BytecodeConstants::LFO, 3));
    }

    void BytecodeTransformer::transform(const Sweep* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        addInstruction(new CallNative(BytecodeConstants::SWEEP, 3));
    }

    void BytecodeTransformer::transform(const Sequence* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "order");
        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::SEQUENCE, 2));
    }

    void BytecodeTransformer::transform(const Repeat* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "repeats");
        transformArgument(token->arguments, "value");

        addInstruction(new CallNative(BytecodeConstants::REPEAT, 2));
    }

    void BytecodeTransformer::transform(const Random* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "type");
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        addInstruction(new CallNative(BytecodeConstants::RANDOM, 4));
    }

    void BytecodeTransformer::transform(const Limit* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "max");
        transformArgument(token->arguments, "min");
        transformArgument(token->arguments, "value");

        addInstruction(new CallNative(BytecodeConstants::LIMIT, 3));
    }

    void BytecodeTransformer::transform(const Trigger* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "value");
        transformArgument(token->arguments, "condition");

        addInstruction(new CallNative(BytecodeConstants::TRIGGER, 2));
    }

    void BytecodeTransformer::transform(const If* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "false");
        transformArgument(token->arguments, "true");
        transformArgument(token->arguments, "condition");

        addInstruction(new CallNative(BytecodeConstants::IF, 3));
    }

    void BytecodeTransformer::transform(const All* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::ALL, 1));
    }

    void BytecodeTransformer::transform(const Any* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::ANY, 1));
    }

    void BytecodeTransformer::transform(const None* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::NONE, 1));
    }

    void BytecodeTransformer::transform(const Min* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::MIN, 1));
    }

    void BytecodeTransformer::transform(const Max* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "values");

        addInstruction(new CallNative(BytecodeConstants::MAX, 1));
    }

    void BytecodeTransformer::transform(const Round* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "direction");
        transformArgument(token->arguments, "step");
        transformArgument(token->arguments, "value");

        addInstruction(new CallNative(BytecodeConstants::ROUND, 3));
    }

    void BytecodeTransformer::transform(const Sine* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::SINE, 4));
    }

    void BytecodeTransformer::transform(const Square* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::SQUARE, 4));
    }

    void BytecodeTransformer::transform(const Triangle* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::TRIANGLE, 4));
    }

    void BytecodeTransformer::transform(const Saw* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::SAW, 4));
    }

    void BytecodeTransformer::transform(const Oscillator* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "waveform");
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::OSCILLATOR, 5));
    }

    void BytecodeTransformer::transform(const Noise* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        addInstruction(new CallNative(BytecodeConstants::NOISE, 3));
    }

    void BytecodeTransformer::transform(const Sample* token)
    {
        token->arguments->check();

        const String* str = dynamic_cast<const String*>(token->arguments->get("file"));

        const Path* path = Path::beside(str->str, sourcePath);

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

    void BytecodeTransformer::transform(const Delay* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "feedback");
        transformArgument(token->arguments, "delay");
        transformArgument(token->arguments, "mix");

        addInstruction(new CallNative(BytecodeConstants::DELAY, 3));
    }

    void BytecodeTransformer::transform(const Perform* token)
    {
        token->arguments->check();

        transformArgument(token->arguments, "interval");
        transformArgument(token->arguments, "repeats");
        transformArgument(token->arguments, "delay");

        InstructionBlock* block = new InstructionBlock();

        blocks.push(block);

        transformArgument(token->arguments, "function");

        blocks.pop();

        resolver->addInstructionBlock(block);

        addInstruction(new StackPushAddress(block));
        addInstruction(new CallNative(BytecodeConstants::PERFORM, 4));
    }

    void BytecodeTransformer::transform(const Include* token)
    {
        token->arguments->check();

        if (token->program)
        {
            for (const Token* instruction : token->program->instructions)
            {
                addInstruction(new ClearStack());

                instruction->transform(this);
            }
        }
    }

    void BytecodeTransformer::transform(const CallUser* token)
    {
        token->arguments->check();

        for (const Identifier* input : token->function->inputs)
        {
            transformArgument(token->arguments, input->str);

            if (input->id >= nextIdentifierId)
            {
                nextIdentifierId = input->id + 1;
            }

            addInstruction(new SetVariable(input->id));
        }

        addInstruction(new ::CallUser(token->function->block, token->function->inputs.size()));
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

        addInstruction(new CallNative(BytecodeConstants::LESSEQUAL, 2));
    }

    void BytecodeTransformer::transform(const GreaterEqualAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        addInstruction(new CallNative(BytecodeConstants::GREATEREQUAL, 2));
    }

    void BytecodeTransformer::transform(const Define* token)
    {
        blocks.push(token->block);

        for (const Token* instruction : token->instructions)
        {
            addInstruction(new ClearStack());

            instruction->transform(this);
        }

        blocks.pop();

        resolver->addInstructionBlock(token->block);

        std::vector<std::pair<std::string, unsigned char>> inputs;

        for (const Identifier* input : token->inputs)
        {
            inputs.push_back(std::make_pair(input->str, input->id));
        }

        std::sort(inputs.begin(), inputs.end(), std::greater());

        for (const std::pair<std::string, unsigned char>& input : inputs)
        {
            addInstruction(new GetVariable(input.second));
        }

        addInstruction(new StackPushAddress(token->block));
        addInstruction(new StackPushInt(token->inputs.size()));
        addInstruction(new CallNative(BytecodeConstants::LAMBDA, 2));
        addInstruction(new SetVariable(token->id));
    }

    void BytecodeTransformer::transform(const Program* token)
    {
        InstructionBlock* block = new InstructionBlock();

        blocks.push(block);

        resolver->addInstructionBlock(block);

        for (const Token* instruction : token->instructions)
        {
            addInstruction(new ClearStack());

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

        addInstruction(new StackPushDefault());
    }

    void BytecodeTransformer::addInstruction(const BytecodeInstruction* instruction)
    {
        blocks.top()->addInstruction(instruction);
    }
}
