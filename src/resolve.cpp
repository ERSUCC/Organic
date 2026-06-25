#include "../include/resolve.h"

using namespace Parser;

void TypeResolver::resolveTypes(InputDef* token)
{
    token->defaultValue->resolveTypes(this);
}

void TypeResolver::resolveTypes(List* token)
{
    for (Token* value : token->values)
    {
        value->resolveTypes(this);

        if (!token->values[0]->type()->checkType(value->type()))
        {
            throw OrganicParseException("All elements in a list must have the same type.", value->location);
        }
    }
}

void TypeResolver::resolveTypes(ParenthesizedExpression* token)
{
    token->value->resolveTypes(this);
}

void TypeResolver::resolveTypes(Assign* token)
{
    token->variable->value->resolveTypes(this);
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
    resolveArgumentTypes(token->arguments, "order", new SequenceOrderType(), new Constant(token->location, new SequenceOrderType(), Constants::Sequence::Forward));
    resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Repeat* token)
{
    resolveArgumentTypes(token->arguments, "repeats", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(Random* token)
{
    resolveArgumentTypes(token->arguments, "type", new RandomTypeType(), new Constant(token->location, new RandomTypeType(), Constants::Random::Step));
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
    resolveArgumentTypes(token->arguments, "direction", new RoundDirectionType(), new Constant(token->location, new RoundDirectionType(), Constants::Round::Nearest));
    resolveArgumentTypes(token->arguments, "step", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(Absolute* token)
{
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(Sine* token)
{
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Square* token)
{
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Triangle* token)
{
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Saw* token)
{
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Oscillator* token)
{
    resolveArgumentTypes(token->arguments, "waveform", new LambdaType({ { "phase", SharedType(new NumberType()) } }, new NumberType()));
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Noise* token)
{
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Sample* token)
{
    resolveArgumentTypes(token->arguments, "file", new StringType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Granulate* token)
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

void TypeResolver::resolveTypes(Group* token)
{
    resolveArgumentTypes(token->arguments, "sources", new ListType(new AudioSourceType()));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(EffectGroup* token)
{
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Delay* token)
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix",new  NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(Comb* token)
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(AllPass* token)
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(LowPass* token)
{
    resolveArgumentTypes(token->arguments, "threshold", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(Reverb* token)
{
    resolveArgumentTypes(token->arguments, "length", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));

    token->arguments->check();
}

void TypeResolver::resolveTypes(CallUser* token)
{
    for (const InputDef* input : token->function->definition->inputs)
    {
        resolveArgumentTypes(token->arguments, input->string(), input->type(), input->defaultValue);
    }

    token->arguments->check();
}

void TypeResolver::resolveTypes(CallAlias* token)
{
    resolveArgumentTypes(token->arguments, "a", new NumberType());
    resolveArgumentTypes(token->arguments, "b", new NumberType());
}

void TypeResolver::resolveTypes(Define* token)
{
    for (InputDef* input : token->function->inputs)
    {
        input->resolveTypes(this);
    }

    for (Token* instruction : token->function->instructions)
    {
        instruction->resolveTypes(this);
    }
}

void TypeResolver::resolveTypes(Program* token)
{
    for (Token* instruction : token->instructions)
    {
        instruction->resolveTypes(this);
    }
}

void TypeResolver::resolveArgumentTypes(ArgumentList* arguments, const std::string name, const SharedType expectedType, Token* defaultValue)
{
    for (Argument* argument : arguments->arguments)
    {
        if (argument->name == name)
        {
            argument->used = true;

            argument->value->resolveTypes(this);

            const SharedType argumentType = argument->value->type();

            if (!expectedType->checkType(argumentType))
            {
                throw OrganicParseException("Expected " + expectedType->name() + " for input \"" + name + "\", but received " + argumentType->name() + ".", argument->value->location);
            }

            return;
        }
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

void TypeResolver::resolveArgumentTypes(ArgumentList* arguments, const std::string name, const Type* expectedType, Token* defaultValue)
{
    resolveArgumentTypes(arguments, name, SharedType(expectedType), defaultValue);
}
