#include "../include/transform.h"

#define ARG(name) transformArgument(token->arguments, name)

TokenTransformer::TokenTransformer(const Path& sourcePath) :
    sourcePath(sourcePath) {}

Engine::ValueObject* TokenTransformer::transform(const Parser::Value* token)
{
    return new Engine::Value(token->value);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Constant* token)
{
    return new Engine::ValueByte(token->value);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::VariableDef* token)
{
    variables[token] = token->value->transform(this);

    return nullptr;
}

Engine::ValueObject* TokenTransformer::transform(const Parser::VariableRef* token)
{
    return variables[token->definition];
}

Engine::ValueObject* TokenTransformer::transform(const Parser::InputRef* token)
{
    return inputs[token->definition];
}

Engine::ValueObject* TokenTransformer::transform(const Parser::FunctionRef* token)
{
    std::vector<Engine::Variable*> placeholders;

    for (const Parser::InputDef* input : token->definition->inputs)
    {
        Engine::Variable* variable = new Engine::Variable(input->defaultValue->transform(this));

        placeholders.push_back(variable);

        inputs[input] = variable;
    }

    for (size_t i = 0; i < token->definition->instructions.size() - 1; i++)
    {
        token->definition->instructions[i]->transform(this);
    }

    Engine::ValueObject* value = token->definition->instructions.back()->transform(this);

    return new Engine::Lambda(placeholders, value);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EmptyLambda* token)
{
    return new Engine::Lambda({}, token->value->transform(this));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::List* token)
{
    std::vector<Engine::ValueObject*> objects;

    for (const Parser::Token* value : token->values)
    {
        objects.push_back(value->transform(this));
    }

    return new Engine::List(objects);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::ParenthesizedExpression* token)
{
    return token->value->transform(this);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Time* token)
{
    return new Engine::Time();
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Hold* token)
{
    return new Engine::Hold(ARG("value"), ARG("length"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::LFO* token)
{
    return new Engine::LFO(ARG("from"), ARG("to"), ARG("length"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sweep* token)
{
    return new Engine::Sweep(ARG("from"), ARG("to"), ARG("length"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sequence* token)
{
    return new Engine::Sequence(ARG("values"), ARG("order"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Repeat* token)
{
    return new Engine::Repeat(ARG("value"), ARG("repeats"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Random* token)
{
    return new Engine::Random(ARG("from"), ARG("to"), ARG("length"), ARG("type"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Limit* token)
{
    return new Engine::Limit(ARG("value"), ARG("min"), ARG("max"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Trigger* token)
{
    return new Engine::Trigger(ARG("condition"), ARG("value"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::If* token)
{
    return new Engine::If(ARG("condition"), ARG("true"), ARG("false"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::All* token)
{
    return new Engine::All(ARG("values"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Any* token)
{
    return new Engine::Any(ARG("values"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::None* token)
{
    return new Engine::None(ARG("values"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Min* token)
{
    return new Engine::Min(ARG("values"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Max* token)
{
    return new Engine::Max(ARG("values"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Round* token)
{
    return new Engine::Round(ARG("value"), ARG("step"), ARG("direction"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Absolute* token)
{
    return new Engine::Absolute(ARG("value"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EmptyAudioSource* token)
{
    return new Engine::AudioSource();
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sine* token)
{
    return new Engine::Sine(ARG("volume"), ARG("pan"), ARG("effects"), ARG("frequency"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Square* token)
{
    return new Engine::Square(ARG("volume"), ARG("pan"), ARG("effects"), ARG("frequency"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Triangle* token)
{
    return new Engine::Triangle(ARG("volume"), ARG("pan"), ARG("effects"), ARG("frequency"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Saw* token)
{
    return new Engine::Saw(ARG("volume"), ARG("pan"), ARG("effects"), ARG("frequency"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Oscillator* token)
{
    return new Engine::CustomOscillator(ARG("volume"), ARG("pan"), ARG("effects"), ARG("frequency"), ARG("waveform"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Noise* token)
{
    return new Engine::Noise(ARG("volume"), ARG("pan"), ARG("effects"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sample* token)
{
    const Parser::Argument* file = token->arguments->findArgument("file");

    const Path path = Path::beside(Path::formatPath(file->value->string()), sourcePath);

    Engine::Resource* resource = new Engine::Resource(path, file->location);

    return new Engine::Sample(ARG("volume"), ARG("pan"), ARG("effects"), resource);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Granulate* token)
{
    const Parser::Argument* file = token->arguments->findArgument("sample");

    const Path path = Path::beside(Path::formatPath(file->value->string()), sourcePath);

    Engine::Resource* resource = new Engine::Resource(path, file->location);

    return new Engine::Granulate(ARG("volume"), ARG("pan"), ARG("effects"), resource, ARG("grains"), ARG("length"), ARG("shape"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Group* token)
{
    return new Engine::Group(ARG("volume"), ARG("pan"), ARG("effects"), ARG("sources"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EmptyEffect* token)
{
    return new Engine::Effect();
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EffectGroup* token)
{
    return new Engine::EffectGroup(ARG("mix"), ARG("effects"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Delay* token)
{
    return new Engine::Delay(ARG("mix"), ARG("delay"), ARG("feedback"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Comb* token)
{
    return new Engine::Comb(ARG("mix"), ARG("delay"), ARG("feedback"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::AllPass* token)
{
    return new Engine::AllPass(ARG("mix"), ARG("delay"), ARG("feedback"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::LowPass* token)
{
    return new Engine::LowPass(ARG("threshold"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Reverb* token)
{
    return new Engine::Reverb(ARG("mix"), ARG("length"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::CallUser* token)
{
    for (const Parser::InputDef* input : token->function->definition->inputs)
    {
        inputs[input] = transformArgument(token->arguments, input->string());
    }

    for (size_t i = 0; i < token->function->definition->instructions.size() - 1; i++)
    {
        token->function->definition->instructions[i]->transform(this);
    }

    return token->function->definition->instructions.back()->transform(this);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::AddAlias* token)
{
    return new Engine::ValueAdd(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::SubtractAlias* token)
{
    return new Engine::ValueSubtract(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::MultiplyAlias* token)
{
    return new Engine::ValueMultiply(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::DivideAlias* token)
{
    return new Engine::ValueDivide(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::PowerAlias* token)
{
    return new Engine::ValuePower(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EqualAlias* token)
{
    return new Engine::ValueEquals(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::LessAlias* token)
{
    return new Engine::ValueLess(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::GreaterAlias* token)
{
    return new Engine::ValueGreater(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::LessEqualAlias* token)
{
    return new Engine::ValueLessEqual(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::GreaterEqualAlias* token)
{
    return new Engine::ValueGreaterEqual(ARG("a"), ARG("b"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Program* token)
{
    std::vector<Engine::AudioSource*> audioSources;

    for (const Parser::Token* instruction : token->instructions)
    {
        if (Engine::AudioSource* audioSource = dynamic_cast<Engine::AudioSource*>(instruction->transform(this)))
        {
            audioSources.push_back(audioSource);
        }
    }

    return new Engine::Program(audioSources);
}

Engine::ValueObject* TokenTransformer::transformArgument(const Parser::ArgumentList* arguments, const std::string name)
{
    for (const Parser::Argument* argument : arguments->arguments)
    {
        if (argument->name == name)
        {
            return argument->value->transform(this);
        }
    }

    return nullptr;
}
