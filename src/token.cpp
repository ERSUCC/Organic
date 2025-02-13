#include "../include/token.h"

namespace Parser
{
    Type::Type(const BasicType primaryType, Type* subType) :
        primaryType(primaryType), subType(subType) {}

    std::string Type::name() const
    {
        switch (primaryType)
        {
            case BasicType::None:
                return "nothing";

            case BasicType::Any:
                return "anything";

            case BasicType::SequenceOrder:
                return "sequence order constant";

            case BasicType::RandomType:
                return "random type constant";

            case BasicType::Number:
                return "number";

            case BasicType::Boolean:
                return "boolean";

            case BasicType::String:
                return "string";

            case BasicType::List:
                return "list of " + subType->name();

            case BasicType::AudioSource:
                return "audio source";

            case BasicType::Effect:
                return "effect";
        }

        return "";
    }

    bool Type::checkType(const Type* expected) const
    {
        if (!expected)
        {
            return false;
        }

        switch (expected->primaryType)
        {
            case BasicType::None:
                if (primaryType != BasicType::None)
                {
                    return false;
                }

                break;

            case BasicType::Any:
                break;

            case BasicType::SequenceOrder:
                if (primaryType != BasicType::SequenceOrder)
                {
                    return false;
                }

                break;

            case BasicType::RandomType:
                if (primaryType != BasicType::RandomType)
                {
                    return false;
                }

                break;

            case BasicType::Number:
                if (primaryType != BasicType::Number)
                {
                    return false;
                }

                break;

            case BasicType::Boolean:
                if (primaryType != BasicType::Boolean)
                {
                    return false;
                }

                break;

            case BasicType::String:
                if (primaryType != BasicType::String)
                {
                    return false;
                }

                break;

            case BasicType::List:
                if (primaryType != BasicType::List)
                {
                    return false;
                }

                break;

            case BasicType::AudioSource:
                if (primaryType != BasicType::AudioSource)
                {
                    return false;
                }

                break;

            case BasicType::Effect:
                if (primaryType != BasicType::Effect)
                {
                    return false;
                }

                break;
        }

        if (subType)
        {
            return subType->checkType(expected->subType);
        }

        return !expected->subType;
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
        type = new Type(BasicType::Number);
    }

    void Value::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SequenceForwards::SequenceForwards(const SourceLocation location) :
        BasicToken(location, "sequence-forwards")
    {
        type = new Type(BasicType::SequenceOrder);
    }

    void SequenceForwards::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SequenceBackwards::SequenceBackwards(const SourceLocation location) :
        BasicToken(location, "sequence-backwards")
    {
        type = new Type(BasicType::SequenceOrder);
    }

    void SequenceBackwards::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SequencePingPong::SequencePingPong(const SourceLocation location) :
        BasicToken(location, "sequence-ping-pong")
    {
        type = new Type(BasicType::SequenceOrder);
    }

    void SequencePingPong::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SequenceRandom::SequenceRandom(const SourceLocation location) :
        BasicToken(location, "sequence-random")
    {
        type = new Type(BasicType::SequenceOrder);
    }

    void SequenceRandom::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    RandomStep::RandomStep(const SourceLocation location) :
        BasicToken(location, "random-step")
    {
        type = new Type(BasicType::RandomType);
    }

    void RandomStep::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    RandomLinear::RandomLinear(const SourceLocation location) :
        BasicToken(location, "random-linear")
    {
        type = new Type(BasicType::RandomType);
    }

    void RandomLinear::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Pi::Pi(const SourceLocation location) :
        BasicToken(location, "pi")
    {
        type = new Type(BasicType::Number);
    }

    void Pi::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    E::E(const SourceLocation location) :
        BasicToken(location, "e")
    {
        type = new Type(BasicType::Number);
    }

    void E::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    String::String(const SourceLocation location, const std::string str) :
        BasicToken(location, str)
    {
        type = new Type(BasicType::String);
    }

    Ignore::Ignore(const SourceLocation location) :
        Token(location) {}

    Argument::Argument(const SourceLocation location, const std::string name, Token* value) :
        Token(location), name(name), value(value) {}

    std::string Argument::string() const
    {
        return name + ": " + value->string();
    }

    ArgumentList::ArgumentList(const std::vector<Argument*>& arguments, const std::string name) :
        arguments(arguments), name(name) {}

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

    Call::Call(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Token(location), arguments(arguments), topLevel(topLevel) {}

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

    Time::Time(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Time::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Hold::Hold(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Hold::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Hold::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    LFO::LFO(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void LFO::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void LFO::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sweep::Sweep(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Sweep::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sweep::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sequence::Sequence(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Sequence::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sequence::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Repeat::Repeat(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Repeat::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Repeat::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Random::Random(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Random::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Random::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Limit::Limit(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Limit::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Limit::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Trigger::Trigger(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Trigger::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Trigger::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    If::If(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void If::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void If::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sine::Sine(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Sine::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sine::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Square::Square(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Square::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Square::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Triangle::Triangle(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Triangle::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Triangle::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Saw::Saw(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Saw::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Saw::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Noise::Noise(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Noise::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Noise::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sample::Sample(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Sample::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sample::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Delay::Delay(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Effect);
    }

    void Delay::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Delay::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Perform::Perform(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::None);
    }

    void Perform::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Perform::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    CallUser::CallUser(const SourceLocation location, const ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel) {}

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
        type = new Type(BasicType::Number);
    }

    void AddAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SubtractAlias::SubtractAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "-")
    {
        type = new Type(BasicType::Number);
    }

    void SubtractAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    MultiplyAlias::MultiplyAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "*")
    {
        type = new Type(BasicType::Number);
    }

    void MultiplyAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    DivideAlias::DivideAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "/")
    {
        type = new Type(BasicType::Number);
    }

    void DivideAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    PowerAlias::PowerAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "^")
    {
        type = new Type(BasicType::Number);
    }

    void PowerAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    EqualAlias::EqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "==")
    {
        type = new Type(BasicType::Boolean);
    }

    void EqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    LessAlias::LessAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "<")
    {
        type = new Type(BasicType::Boolean);
    }

    void LessAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    GreaterAlias::GreaterAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, ">")
    {
        type = new Type(BasicType::Boolean);
    }

    void GreaterAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    LessEqualAlias::LessEqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, "<=")
    {
        type = new Type(BasicType::Boolean);
    }

    void LessEqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    GreaterEqualAlias::GreaterEqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b, ">=")
    {
        type = new Type(BasicType::Boolean);
    }

    void GreaterEqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Define::Define(const SourceLocation location, const std::string name, const std::vector<Identifier*> inputs, const std::vector<Token*> instructions) :
        Token(location), name(name), inputs(inputs), instructions(instructions)
    {
        type = new Type(BasicType::None);

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
        type = new Type(BasicType::None);
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

    Include::Include(const SourceLocation location, Program* program) :
        Token(location), program(program)
    {
        type = new Type(BasicType::None);
    }

    void Include::resolveTypes(TypeResolver* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Include::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    TypeResolver::TypeResolver(const Path* sourcePath) :
        sourcePath(sourcePath) {}

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

        else
        {
            throw OrganicParseException("Unrecognized variable name \"" + token->str + "\".", token->location);
        }
    }

    void TypeResolver::resolveTypes(List* token)
    {
        Type* innerType = expectedType->subType;

        for (Token* value : token->values)
        {
            expectedType = innerType;

            value->resolveTypes(this);

            if (!value->type->checkType(token->values[0]->type))
            {
                throw OrganicParseException("All elements in a list must have the same type.", value->location);
            }
        }

        token->type = new Type(BasicType::List, token->values[0]->type);
    }

    void TypeResolver::resolveTypes(ParenthesizedExpression* token)
    {
        token->value->resolveTypes(this);

        token->type = token->value->type;
    }

    void TypeResolver::resolveTypes(Assign* token)
    {
        token->value->resolveTypes(this);

        if (token->value->type->checkType(new Type(BasicType::None)))
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

        token->variable->type = token->value->type;
        token->variable->id = nextIdentifierId++;

        addVariable(token->variable);
    }

    void TypeResolver::resolveTypes(Hold* token)
    {
        resolveArgumentTypes(token->arguments, "length", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "value", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(LFO* token)
    {
        resolveArgumentTypes(token->arguments, "length", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "to", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "from", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Sweep* token)
    {
        resolveArgumentTypes(token->arguments, "length", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "to", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "from", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Sequence* token)
    {
        resolveArgumentTypes(token->arguments, "order", new Type(BasicType::SequenceOrder));
        resolveArgumentTypes(token->arguments, "values", new Type(BasicType::List, new Type(BasicType::Number)));
    }

    void TypeResolver::resolveTypes(Repeat* token)
    {
        resolveArgumentTypes(token->arguments, "repeats", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "value", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Random* token)
    {
        resolveArgumentTypes(token->arguments, "type", new Type(BasicType::RandomType));
        resolveArgumentTypes(token->arguments, "length", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "to", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "from", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Limit* token)
    {
        resolveArgumentTypes(token->arguments, "max", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "min", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "value", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Trigger* token)
    {
        resolveArgumentTypes(token->arguments, "value", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "condition", new Type(BasicType::Boolean));
    }

    void TypeResolver::resolveTypes(If* token)
    {
        resolveArgumentTypes(token->arguments, "false", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "true", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "condition", new Type(BasicType::Boolean));
    }

    void TypeResolver::resolveTypes(Sine* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "effects", new Type(BasicType::List, new Type(BasicType::Effect)));
        resolveArgumentTypes(token->arguments, "pan", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "volume", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Square* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "effects", new Type(BasicType::List, new Type(BasicType::Effect)));
        resolveArgumentTypes(token->arguments, "pan", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "volume", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Triangle* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "effects", new Type(BasicType::List, new Type(BasicType::Effect)));
        resolveArgumentTypes(token->arguments, "pan", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "volume", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Saw* token)
    {
        resolveArgumentTypes(token->arguments, "frequency", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "effects", new Type(BasicType::List, new Type(BasicType::Effect)));
        resolveArgumentTypes(token->arguments, "pan", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "volume", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Noise* token)
    {
        resolveArgumentTypes(token->arguments, "effects", new Type(BasicType::List, new Type(BasicType::Effect)));
        resolveArgumentTypes(token->arguments, "pan", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "volume", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Sample* token)
    {
        resolveArgumentTypes(token->arguments, "file", new Type(BasicType::String));
        resolveArgumentTypes(token->arguments, "effects", new Type(BasicType::List, new Type(BasicType::Effect)));
        resolveArgumentTypes(token->arguments, "pan", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "volume", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Delay* token)
    {
        resolveArgumentTypes(token->arguments, "feedback", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "delay", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "mix", new Type(BasicType::Number));
    }

    void TypeResolver::resolveTypes(Perform* token)
    {
        resolveArgumentTypes(token->arguments, "interval", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "repeats", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "delay", new Type(BasicType::Number));
        resolveArgumentTypes(token->arguments, "function", new Type(BasicType::Any));
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
                    resolveArgumentTypes(token->arguments, input->str, new Type(BasicType::Any));
                }
            }

            token->function = function;
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
        token->a->resolveTypes(this);
        token->b->resolveTypes(this);

        const Type* number = new Type(BasicType::Number);

        if (!token->a->type->checkType(number))
        {
            throw OrganicParseException("Expected \"" + number->name() + "\", received \"" + token->a->type->name() + "\".", token->a->location);
        }

        if (!token->b->type->checkType(number))
        {
            throw OrganicParseException("Expected \"" + number->name() + "\", received \"" + token->b->type->name() + "\".", token->b->location);
        }
    }

    void TypeResolver::resolveTypes(Define* token)
    {
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
            // should this be a warning?

            token->type = new Type(BasicType::None);
        }

        else
        {
            token->type = token->instructions.back()->type;
        }
    }

    void TypeResolver::resolveTypes(Program* token)
    {
        scopes.push_back(new Scope("", {}));

        for (Token* instruction : token->instructions)
        {
            instruction->resolveTypes(this);
        }

        checkUses();

        scopes.pop_back();
    }

    void TypeResolver::resolveTypes(Include* token)
    {
        for (Token* instruction : token->program->instructions)
        {
            instruction->resolveTypes(this);
        }
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

                if (!argumentType->checkType(expectedType))
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
        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::TIME, 0));
    }

    void BytecodeTransformer::transform(const Hold* token)
    {
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "value");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::HOLD, 2));
    }

    void BytecodeTransformer::transform(const LFO* token)
    {
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::LFO, 3));
    }

    void BytecodeTransformer::transform(const Sweep* token)
    {
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::SWEEP, 3));
    }

    void BytecodeTransformer::transform(const Sequence* token)
    {
        transformArgument(token->arguments, "order");
        transformArgument(token->arguments, "values");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::SEQUENCE, 2));
    }

    void BytecodeTransformer::transform(const Repeat* token)
    {
        transformArgument(token->arguments, "repeats");
        transformArgument(token->arguments, "value");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::REPEAT, 2));
    }

    void BytecodeTransformer::transform(const Random* token)
    {
        transformArgument(token->arguments, "type");
        transformArgument(token->arguments, "length");
        transformArgument(token->arguments, "to");
        transformArgument(token->arguments, "from");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::RANDOM, 4));
    }

    void BytecodeTransformer::transform(const Limit* token)
    {
        transformArgument(token->arguments, "max");
        transformArgument(token->arguments, "min");
        transformArgument(token->arguments, "value");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::LIMIT, 3));
    }

    void BytecodeTransformer::transform(const Trigger* token)
    {
        transformArgument(token->arguments, "value");
        transformArgument(token->arguments, "condition");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::TRIGGER, 2));
    }

    void BytecodeTransformer::transform(const If* token)
    {
        transformArgument(token->arguments, "false");
        transformArgument(token->arguments, "true");
        transformArgument(token->arguments, "condition");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::IF, 3));
    }

    void BytecodeTransformer::transform(const Sine* token)
    {
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::SINE, 4));

        if (token->topLevel)
        {
            addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Square* token)
    {
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::SQUARE, 4));

        if (token->topLevel)
        {
            addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Triangle* token)
    {
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::TRIANGLE, 4));

        if (token->topLevel)
        {
            addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Saw* token)
    {
        transformArgument(token->arguments, "frequency");
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::SAW, 4));

        if (token->topLevel)
        {
            addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Noise* token)
    {
        transformArgument(token->arguments, "effects");
        transformArgument(token->arguments, "pan");
        transformArgument(token->arguments, "volume");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::NOISE, 3));

        if (token->topLevel)
        {
            addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Sample* token)
    {
        const String* str = dynamic_cast<const String*>(getArgument(token->arguments, "file"));

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

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::SAMPLE, 4));

        if (token->topLevel)
        {
            addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Delay* token)
    {
        transformArgument(token->arguments, "feedback");
        transformArgument(token->arguments, "delay");
        transformArgument(token->arguments, "mix");

        checkArguments(token->arguments);

        addInstruction(new CallNative(BytecodeConstants::DELAY, 3));
    }

    void BytecodeTransformer::transform(const Perform* token)
    {
        transformArgument(token->arguments, "interval");
        transformArgument(token->arguments, "repeats");
        transformArgument(token->arguments, "delay");

        InstructionBlock* block = new InstructionBlock();

        blocks.push(block);

        transformArgument(token->arguments, "function");

        checkArguments(token->arguments);

        blocks.pop();

        resolver->addInstructionBlock(block);

        addInstruction(new StackPushAddress(block));
        addInstruction(new CallNative(BytecodeConstants::PERFORM, 4));
    }

    void BytecodeTransformer::transform(const CallUser* token)
    {
        for (const Identifier* input : token->function->inputs)
        {
            transformArgument(token->arguments, input->str);

            if (input->id >= nextIdentifierId)
            {
                nextIdentifierId = input->id + 1;
            }

            addInstruction(new SetVariable(input->id));
        }

        checkArguments(token->arguments);

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
            instruction->transform(this);
        }

        blocks.pop();

        resolver->addInstructionBlock(token->block);
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

    void BytecodeTransformer::transform(const Include* token)
    {
        for (const Token* instruction : token->program->instructions)
        {
            instruction->transform(this);
        }
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

    Token* BytecodeTransformer::getArgument(const ArgumentList* arguments, const std::string name) const
    {
        for (unsigned int i = 0; i < arguments->arguments.size(); i++)
        {
            if (arguments->arguments[i]->name == name)
            {
                return arguments->arguments[i]->value;
            }
        }

        return nullptr;
    }

    void BytecodeTransformer::checkArguments(const ArgumentList* arguments) const
    {
        for (const Argument* argument : arguments->arguments)
        {
            if (!argument->used)
            {
                throw OrganicParseException("Invalid input name \"" + argument->name + "\" for function \"" + arguments->name + "\".", argument->location);
            }
        }
    }

    void BytecodeTransformer::addInstruction(const BytecodeInstruction* instruction)
    {
        blocks.top()->addInstruction(instruction);
    }
}
