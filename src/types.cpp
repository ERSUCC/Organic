#include "../include/types.h"

using namespace Parser;

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

Token* Type::getDefault(const SourceLocation location) const
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

Token* SequenceOrderType::getDefault(const SourceLocation location) const
{
    return new SequenceForward(location);
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

Token* RandomTypeType::getDefault(const SourceLocation location) const
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

Token* RoundDirectionType::getDefault(const SourceLocation location) const
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

Token* AudioSourceType::getDefault(const SourceLocation location) const
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

Token* EffectType::getDefault(const SourceLocation location) const
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

Token* ListType::getDefault(const SourceLocation location) const
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

Token* LambdaType::getDefault(const SourceLocation location) const
{
    return new EmptyLambda(location, returnType->getDefault(location));
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

TypeResolver::TypeResolver(const Path* sourcePath) :
    sourcePath(sourcePath) {}

void TypeResolver::resolveTypes(VariableRef* token)
{
    token->type = token->definition->type;
    token->definition->used = true;
}

void TypeResolver::resolveTypes(InputDef* token)
{
    token->defaultValue->resolveTypes(this);
    token->type = token->defaultValue->type;
}

void TypeResolver::resolveTypes(InputRef* token)
{
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
    for (Token* value : token->values)
    {
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

void TypeResolver::resolveTypes(Absolute* token)
{
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

void TypeResolver::resolveTypes(Granulate* token)
{
    EmptyLambda* defaultLambda = new EmptyLambda(token->arguments->location, new Value(token->arguments->location, "1", 1));

    resolveArgumentTypes(token->arguments, "shape", new LambdaType({ { "value", new NumberType() } }, new NumberType()), defaultLambda);
    resolveArgumentTypes(token->arguments, "length", new NumberType());
    resolveArgumentTypes(token->arguments, "grains", new NumberType());
    resolveArgumentTypes(token->arguments, "sample", new StringType());
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
    resolveArgumentTypes(token->arguments, "pan", new NumberType());
    resolveArgumentTypes(token->arguments, "volume", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(Group* token)
{
    resolveArgumentTypes(token->arguments, "sources", new ListType(new AudioSourceType()));
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
    resolveArgumentTypes(token->arguments, "pan", new NumberType());
    resolveArgumentTypes(token->arguments, "volume", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(EffectGroup* token)
{
    resolveArgumentTypes(token->arguments, "effects", new ListType(new EffectType()));
    resolveArgumentTypes(token->arguments, "mix", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(Delay* token)
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(Comb* token)
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(AllPass* token)
{
    resolveArgumentTypes(token->arguments, "feedback", new NumberType());
    resolveArgumentTypes(token->arguments, "delay", new NumberType());
    resolveArgumentTypes(token->arguments, "mix", new NumberType());

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
    resolveArgumentTypes(token->arguments, "mix", new NumberType());

    token->arguments->check();
}

void TypeResolver::resolveTypes(CallUser* token)
{
    for (const InputDef* input : token->function->definition->inputs)
    {
        resolveArgumentTypes(token->arguments, input->str, input->type, input->defaultValue);
    }

    token->arguments->check();

    token->type = token->function->definition->returnType;
    token->function->definition->used = true;
}

void TypeResolver::resolveTypes(CallAlias* token)
{
    resolveArgumentTypes(token->arguments, "a", new NumberType());
    resolveArgumentTypes(token->arguments, "b", new NumberType());
}

void TypeResolver::resolveTypes(Define* token)
{
    std::unordered_map<std::string, const Type*> inputTypes;

    for (InputDef* input : token->function->inputs)
    {
        input->resolveTypes(this);

        inputTypes[input->str] = input->type;
    }

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

    token->function->type = new LambdaType(inputTypes, token->type);
    token->function->returnType = token->type;
}

void TypeResolver::resolveTypes(Program* token)
{
    for (Token* instruction : token->instructions)
    {
        instruction->resolveTypes(this);
    }
}

void TypeResolver::resolveArgumentTypes(ArgumentList* arguments, const std::string name, const Type* expectedType, Token* defaultValue)
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

            argument->value->resolveTypes(this);

            const Type* argumentType = argument->value->type;

            if (!expectedType->checkType(argumentType))
            {
                throw OrganicParseException("Expected \"" + expectedType->name() + "\", received \"" + argumentType->name() + "\".", argument->value->location);
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
        arguments->addDefault(name, expectedType->getDefault(arguments->location));
    }
}
