#include "../include/resolve.h"

using namespace Parser;

void TypeResolver::resolveTypes(const VariableDef* token) const
{
    token->value->resolveTypes(this);
}

void TypeResolver::resolveTypes(const InputDef* token) const
{
    token->defaultValue->resolveTypes(this);
}

void TypeResolver::resolveTypes(const FunctionDef* token) const
{
    if (token->program->instructions.empty())
    {
        throw OrganicParseException("The function \"" + token->string() + "\" does not return a value.", token->location);
    }

    for (const InputDef* input : token->inputs)
    {
        input->resolveTypes(this);
    }

    const SharedType noneType(new NoneType());

    for (size_t i = 0; i < token->program->instructions.size() - 1; i++)
    {
        const Token* instruction = token->program->instructions[i];

        instruction->resolveTypes(this);

        if (!noneType->checkType(instruction->type()))
        {
            Utils::parseWarning("This instruction has no effect, it will be ignored.", instruction->location);
        }
    }

    token->program->instructions.back()->resolveTypes(this);

    if (SharedType(new NoneType())->checkType(token->returnType()))
    {
        throw OrganicParseException("The function \"" + token->string() + "\" does not return a value.", token->location);
    }
}

void TypeResolver::resolveTypes(const List* token) const
{
    for (const Token* value : token->values)
    {
        value->resolveTypes(this);

        if (!token->values[0]->type()->checkType(value->type()))
        {
            throw OrganicParseException("All elements in a list must have the same type.", value->location);
        }
    }
}

void TypeResolver::resolveTypes(const ParenthesizedExpression* token) const
{
    token->value->resolveTypes(this);
}

void TypeResolver::resolveTypes(const Time* token) const
{
    token->arguments->check();
}

void TypeResolver::resolveTypes(const Hold* token) const
{
    resolveArgumentTypes(token->arguments, "length", new NumberType());
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const LFO* token) const
{
    resolveArgumentTypes(token->arguments, "length", new NumberType());
    resolveArgumentTypes(token->arguments, "to", new NumberType());
    resolveArgumentTypes(token->arguments, "from", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Sweep* token) const
{
    resolveArgumentTypes(token->arguments, "length", new NumberType());
    resolveArgumentTypes(token->arguments, "to", new NumberType());
    resolveArgumentTypes(token->arguments, "from", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Sequence* token) const
{
    resolveArgumentTypes(token->arguments, "order", new SequenceOrderType(), new Constant(token->location, new SequenceOrderType(), Constants::Sequence::Forward));
    resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Repeat* token) const
{
    resolveArgumentTypes(token->arguments, "repeats", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Random* token) const
{
    resolveArgumentTypes(token->arguments, "type", new RandomTypeType(), new Constant(token->location, new RandomTypeType(), Constants::Random::Step));
    resolveArgumentTypes(token->arguments, "length", new NumberType());
    resolveArgumentTypes(token->arguments, "to", new NumberType());
    resolveArgumentTypes(token->arguments, "from", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Limit* token) const
{
    resolveArgumentTypes(token->arguments, "max", new NumberType());
    resolveArgumentTypes(token->arguments, "min", new NumberType());
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Trigger* token) const
{
    resolveArgumentTypes(token->arguments, "value", new NumberType());
    resolveArgumentTypes(token->arguments, "condition", new BooleanType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const If* token) const
{
    resolveArgumentTypes(token->arguments, "false", new NumberType());
    resolveArgumentTypes(token->arguments, "true", new NumberType());
    resolveArgumentTypes(token->arguments, "condition", new BooleanType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const All* token) const
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Any* token) const
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const None* token) const
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Min* token) const
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Max* token) const
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Round* token) const
{
    resolveArgumentTypes(token->arguments, "direction", new RoundDirectionType(), new Constant(token->location, new RoundDirectionType(), Constants::Round::Nearest));
    resolveArgumentTypes(token->arguments, "step", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Absolute* token) const
{
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Sine* token) const
{
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Square* token) const
{
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Triangle* token) const
{
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Saw* token) const
{
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Oscillator* token) const
{
    resolveArgumentTypes(token->arguments, "waveform", new LambdaType({ { "phase", SharedType(new NumberType()) } }, new NumberType()));
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Noise* token) const
{
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Sample* token) const
{
    resolveArgumentTypes(token->arguments, "file", new StringType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Granulate* token) const
{
    EmptyLambda* defaultLambda = new EmptyLambda(token->arguments->location, new Value(token->arguments->location, 1));

    resolveArgumentTypes(token->arguments, "shape", new LambdaType({ { "value", SharedType(new NumberType()) } }, new NumberType()), defaultLambda);
    resolveArgumentTypes(token->arguments, "length", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "grains", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "sample", new StringType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Group* token) const
{
    resolveArgumentTypes(token->arguments, "sources", new ListType(new AudioSourceType()));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const EffectGroup* token) const
{
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Delay* token) const
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix",new  NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Comb* token) const
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const AllPass* token) const
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const LowPass* token) const
{
    resolveArgumentTypes(token->arguments, "threshold", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Reverb* token) const
{
    resolveArgumentTypes(token->arguments, "length", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const CallUser* token) const
{
    for (const InputDef* input : token->function->inputs)
    {
        resolveArgumentTypes(token->arguments, input->string(), input->type(), input->defaultValue);
    }

    token->arguments->check();
}

void TypeResolver::resolveTypes(const CallAlias* token) const
{
    const SharedType expected = SharedType(new NumberType());

    if (const Argument* argument = token->arguments->findArgument("a"))
    {
        argument->value->resolveTypes(this);

        const SharedType argumentType = argument->value->type();

        if (!expected->checkType(argumentType))
        {
            throw OrganicParseException("Expected " + expected->name() + " on left-hand side, but received " + argumentType->name() + ".", argument->value->location);
        }
    }

    if (const Argument* argument = token->arguments->findArgument("b"))
    {
        argument->value->resolveTypes(this);

        const SharedType argumentType = argument->value->type();

        if (!expected->checkType(argumentType))
        {
            throw OrganicParseException("Expected " + expected->name() + " on right-hand side, but received " + argumentType->name() + ".", argument->value->location);
        }
    }
}

void TypeResolver::resolveTypes(const Program* token) const
{
    const SharedType noneType(new NoneType());
    const SharedType sourceType(new AudioSourceType());

    for (const Token* instruction : token->instructions)
    {
        instruction->resolveTypes(this);

        if (!noneType->checkType(instruction->type()) && !sourceType->checkType(instruction->type()))
        {
            Utils::parseWarning("This instruction has no effect, it will be ignored.", instruction->location);
        }
    }
}

void TypeResolver::resolveArgumentTypes(ArgumentList* arguments, const std::string& name, const SharedType& expectedType, const SharedToken& defaultValue) const
{
    if (const Argument* argument = arguments->findArgument(name))
    {
        argument->value->resolveTypes(this);

        const SharedType argumentType = argument->value->type();

        if (!expectedType->checkType(argumentType))
        {
            throw OrganicParseException("Expected " + expectedType->name() + " for input \"" + name + "\", but received " + argumentType->name() + ".", argument->value->location);
        }

        return;
    }

    if (defaultValue)
    {
        arguments->addDefault(name, defaultValue);
    }

    else
    {
        throw OrganicParseException("Input \"" + name + "\" is required for function \"" + arguments->name + "\".", arguments->location);
    }
}

void TypeResolver::resolveArgumentTypes(ArgumentList* arguments, const std::string& name, const Type* expectedType, const Token* defaultValue) const
{
    resolveArgumentTypes(arguments, name, SharedType(expectedType), SharedToken(defaultValue));
}
