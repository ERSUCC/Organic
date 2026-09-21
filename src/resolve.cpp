#include "../include/resolve.h"

using namespace Parser;

TypeResolver::~TypeResolver()
{
    while (context)
    {
        const FillContext* current = context;

        context = context->parent;

        delete current;
    }
}

void TypeResolver::resolveTypes(const VariableDef* token)
{
    token->value->resolveTypes(this);
}

void TypeResolver::resolveTypes(const InputDef* token)
{
    token->defaultValue->resolveTypes(this);
}

void TypeResolver::resolveTypes(const FunctionDef* token)
{
    if (token->program->instructions.empty())
    {
        throw OrganicParseException("The function \"" + token->string() + "\" does not return a value.", token->location);
    }

    for (const InputDef* input : token->inputs)
    {
        input->resolveTypes(this);
    }

    const UniqueType noneType(new NoneType());

    for (size_t i = 0; i < token->program->instructions.size() - 1; i++)
    {
        const Token* instruction = token->program->instructions[i];

        instruction->resolveTypes(this);

        if (!noneType->checkType(context, instruction->type().get()))
        {
            Utils::parseWarning("This instruction has no effect, it will be ignored.", instruction->location);
        }
    }

    token->program->instructions.back()->resolveTypes(this);

    if (noneType->checkType(context, token->returnType().get()))
    {
        throw OrganicParseException("The function \"" + token->string() + "\" does not return a value.", token->location);
    }
}

void TypeResolver::resolveTypes(const List* token)
{
    for (const Token* value : token->values)
    {
        value->resolveTypes(this);

        if (!token->values[0]->type()->checkType(context, value->type().get()))
        {
            throw OrganicParseException("All elements in a list must have the same type.", value->location);
        }
    }
}

void TypeResolver::resolveTypes(const ParenthesizedExpression* token)
{
    token->value->resolveTypes(this);
}

void TypeResolver::resolveTypes(const Negate* token)
{
    token->value->resolveTypes(this);

    if (!UniqueType(new NumberType())->checkType(context, token->value->type().get()))
    {
        throw OrganicParseException("Expected number, but received " + token->value->type()->name() + ".", token->value->location);
    }
}

void TypeResolver::resolveTypes(const Time* token)
{
    token->arguments->check();
}

void TypeResolver::resolveTypes(const Hold* token)
{
    resolveArgumentTypes(token->arguments, "value", new AnyType());
    resolveArgumentTypes(token->arguments, "length", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const LFO* token)
{
    resolveArgumentTypes(token->arguments, "from", new NumberType());
    resolveArgumentTypes(token->arguments, "to", new NumberType());
    resolveArgumentTypes(token->arguments, "length", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Sweep* token)
{
    resolveArgumentTypes(token->arguments, "from", new NumberType());
    resolveArgumentTypes(token->arguments, "to", new NumberType());
    resolveArgumentTypes(token->arguments, "length", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Sequence* token)
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new AnyType()));
    resolveArgumentTypes(token->arguments, "order", new SequenceOrderType(), new Constant(token->location, new SequenceOrderType(), Constants::Sequence::Forward));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Repeat* token)
{
    resolveArgumentTypes(token->arguments, "value", new AnyType());
    resolveArgumentTypes(token->arguments, "repeats", new NumberType(), new Value(token->location, 0));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Random* token)
{
    resolveArgumentTypes(token->arguments, "from", new NumberType());
    resolveArgumentTypes(token->arguments, "to", new NumberType());
    resolveArgumentTypes(token->arguments, "length", new NumberType());
    resolveArgumentTypes(token->arguments, "type", new RandomTypeType(), new Constant(token->location, new RandomTypeType(), Constants::Random::Stay));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Limit* token)
{
    resolveArgumentTypes(token->arguments, "value", new NumberType());
    resolveArgumentTypes(token->arguments, "min", new NumberType());
    resolveArgumentTypes(token->arguments, "max", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Trigger* token)
{
    resolveArgumentTypes(token->arguments, "condition", new BooleanType());
    resolveArgumentTypes(token->arguments, "value", new AnyType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const If* token)
{
    resolveArgumentTypes(token->arguments, "condition", new BooleanType());
    resolveArgumentTypes(token->arguments, "is-true", new AnyType());
    resolveArgumentTypes(token->arguments, "is-false", new AnyType());

    const SharedToken trueValue = token->arguments->findArgument("is-true")->value;
    const SharedToken falseValue = token->arguments->findArgument("is-false")->value;

    if (!trueValue->type()->checkType(context, falseValue->type().get()))
    {
        throw OrganicParseException("The type of \"is-false\" must match the type of \"is-true\", which is a " + trueValue->type()->name(), falseValue->location);
    }

    token->arguments->check();
}

void TypeResolver::resolveTypes(const All* token)
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Any* token)
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const None* token)
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new BooleanType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Min* token)
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Max* token)
{
    resolveArgumentTypes(token->arguments, "values", new ListType(new NumberType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Round* token)
{
    resolveArgumentTypes(token->arguments, "value", new NumberType());
    resolveArgumentTypes(token->arguments, "step", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "direction", new RoundDirectionType(), new Constant(token->location, new RoundDirectionType(), Constants::Round::Nearest));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Absolute* token)
{
    resolveArgumentTypes(token->arguments, "value", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Modulo* token)
{
    resolveArgumentTypes(token->arguments, "value", new NumberType());
    resolveArgumentTypes(token->arguments, "divisor", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Logarithm* token)
{
    resolveArgumentTypes(token->arguments, "value", new NumberType());
    resolveArgumentTypes(token->arguments, "base", new NumberType(), new Value(token->location, 10));
}

void TypeResolver::resolveTypes(const Sine* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Square* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Triangle* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Saw* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Oscillator* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "frequency", new NumberType());
    resolveArgumentTypes(token->arguments, "waveform", new NumberType(), nullptr, { { "phase", SharedType(new NumberType()) } });
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Noise* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Sample* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "file", new StringType());
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Granulate* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "sample", new StringType());
    resolveArgumentTypes(token->arguments, "shape", new NumberType(), new Value(token->location, 1), { { "position", SharedType(new NumberType()) } });
    resolveArgumentTypes(token->arguments, "length", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "grains", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Group* token)
{
    resolveArgumentTypes(token->arguments, "volume", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "sources", new ListType(new AudioSourceType()));
    resolveArgumentTypes(token->arguments, "pan", new NumberType(), new Value(token->location, 0));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()), new List(token->location, { new EmptyEffect(token->location) }));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const EffectGroup* token)
{
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Delay* token)
{
    resolveArgumentTypes(token->arguments, "mix",new  NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Comb* token)
{
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const AllPass* token)
{
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const LowPass* token)
{
    resolveArgumentTypes(token->arguments, "threshold", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const Reverb* token)
{
    resolveArgumentTypes(token->arguments, "mix", new NumberType(), new Value(token->location, 1));
    resolveArgumentTypes(token->arguments, "length", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(const CallUser* token)
{
    for (const InputDef* input : token->function->inputs)
    {
        resolveArgumentTypes(token->arguments, input->string(), input->type(), input->defaultValue, {});
    }

    token->arguments->check();
}

void TypeResolver::resolveTypes(const CallAlias* token)
{
    const UniqueType expected(new NumberType());

    if (const Argument* argument = token->arguments->findArgument("a"))
    {
        argument->value->resolveTypes(this);

        const SharedType argumentType = argument->value->type();

        if (!expected->checkType(context, argumentType.get()))
        {
            throw OrganicParseException("Expected " + expected->name() + " on left-hand side, but received " + argumentType->name() + ".", argument->value->location);
        }
    }

    if (const Argument* argument = token->arguments->findArgument("b"))
    {
        argument->value->resolveTypes(this);

        const SharedType argumentType = argument->value->type();

        if (!expected->checkType(context, argumentType.get()))
        {
            throw OrganicParseException("Expected " + expected->name() + " on right-hand side, but received " + argumentType->name() + ".", argument->value->location);
        }
    }
}

void TypeResolver::resolveTypes(const Program* token)
{
    const UniqueType noneType(new NoneType());
    const UniqueType sourceType(new AudioSourceType());

    for (const Token* instruction : token->instructions)
    {
        instruction->resolveTypes(this);

        if (!noneType->checkType(context, instruction->type().get()) && !sourceType->checkType(context, instruction->type().get()))
        {
            Utils::parseWarning("This instruction has no effect, it will be ignored.", instruction->location);
        }
    }
}

void TypeResolver::resolveArgumentTypes(ArgumentList* arguments, const std::string& name, const SharedType& expectedType, const SharedToken& defaultValue, const FillTypes& types)
{
    if (const Argument* argument = arguments->findArgument(name))
    {
        context = new FillContext(context, types);

        argument->value->resolveTypes(this);

        const SharedType argumentType = argument->value->type();

        if (!expectedType->checkType(context, argumentType.get()))
        {
            if (argumentType->baseType() != TypeConstant::Fillable)
            {
                throw OrganicParseException("Expected " + expectedType->name() + " for input \"" + name + "\", but received " + argumentType->name() + ".", argument->value->location);
            }

            const FillableType* fillable = dynamic_cast<const FillableType*>(argumentType.get());

            if (const SharedType type = context->findType(fillable->input))
            {
                throw OrganicParseException("Expected " + expectedType->name() + " for input \"" + name + "\", but received " + type->name() + ".", argument->value->location);
            }

            throw OrganicParseException("There is no fillable value available with the name \"" + fillable->input + "\".", argument->value->location);
        }

        FillContext* current = context;

        context = context->parent;

        delete current;

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

void TypeResolver::resolveArgumentTypes(ArgumentList* arguments, const std::string& name, const Type* expectedType, const Token* defaultValue, const FillTypes& types)
{
    resolveArgumentTypes(arguments, name, SharedType(expectedType), SharedToken(defaultValue), types);
}
