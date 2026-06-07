#include "../include/transform.h"

TokenTransformer::TokenTransformer(const Path* sourcePath) :
    sourcePath(sourcePath), utils(Utils::get()) {}

Engine::ValueObject* TokenTransformer::transform(const Parser::Value* token)
{
    return new Engine::Value(token->value);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Constant* token)
{
    return new Engine::ValueByte(token->value);
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

    for (Parser::InputDef* input : token->definition->inputs)
    {
        Engine::Variable* variable = new Engine::Variable(input->defaultValue->transform(this));

        placeholders.push_back(variable);

        inputs[input] = variable;
    }

    const Parser::Define* function = functions[token->definition];

    for (size_t i = 0; i < function->instructions.size() - 1; i++)
    {
        function->instructions[i]->transform(this);
    }

    Engine::ValueObject* value = function->instructions.back()->transform(this);

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

Engine::ValueObject* TokenTransformer::transform(const Parser::Assign* token)
{
    variables[token->variable] = token->value->transform(this);

    return nullptr;
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Time* token)
{
    return new Engine::Time();
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Hold* token)
{
    Engine::ValueObject* value = transformArgument(token->arguments, "value");
    Engine::ValueObject* length = transformArgument(token->arguments, "length");

    return new Engine::Hold(value, length);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::LFO* token)
{
    Engine::ValueObject* from = transformArgument(token->arguments, "from");
    Engine::ValueObject* to = transformArgument(token->arguments, "to");
    Engine::ValueObject* length = transformArgument(token->arguments, "length");

    return new Engine::LFO(from, to, length);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sweep* token)
{
    Engine::ValueObject* from = transformArgument(token->arguments, "from");
    Engine::ValueObject* to = transformArgument(token->arguments, "to");
    Engine::ValueObject* length = transformArgument(token->arguments, "length");

    return new Engine::Sweep(from, to, length);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sequence* token)
{
    Engine::ValueObject* values = transformArgument(token->arguments, "values");
    Engine::ValueObject* order = transformArgument(token->arguments, "order");

    return new Engine::Sequence(values, order);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Repeat* token)
{
    Engine::ValueObject* value = transformArgument(token->arguments, "value");
    Engine::ValueObject* repeats = transformArgument(token->arguments, "repeats");

    return new Engine::Repeat(value, repeats);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Random* token)
{
    Engine::ValueObject* from = transformArgument(token->arguments, "from");
    Engine::ValueObject* to = transformArgument(token->arguments, "to");
    Engine::ValueObject* length = transformArgument(token->arguments, "length");
    Engine::ValueObject* type = transformArgument(token->arguments, "type");

    return new Engine::Random(from, to, length, type);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Limit* token)
{
    Engine::ValueObject* value = transformArgument(token->arguments, "value");
    Engine::ValueObject* min = transformArgument(token->arguments, "min");
    Engine::ValueObject* max = transformArgument(token->arguments, "max");

    return new Engine::Limit(value, min, max);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Trigger* token)
{
    Engine::ValueObject* condition = transformArgument(token->arguments, "condition");
    Engine::ValueObject* value = transformArgument(token->arguments, "value");

    return new Engine::Trigger(condition, value);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::If* token)
{
    Engine::ValueObject* condition = transformArgument(token->arguments, "condition");
    Engine::ValueObject* trueValue = transformArgument(token->arguments, "true");
    Engine::ValueObject* falseValue = transformArgument(token->arguments, "false");

    return new Engine::If(condition, trueValue, falseValue);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::All* token)
{
    Engine::ValueObject* values = transformArgument(token->arguments, "values");

    return new Engine::All(values);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Any* token)
{
    Engine::ValueObject* values = transformArgument(token->arguments, "values");

    return new Engine::Any(values);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::None* token)
{
    Engine::ValueObject* values = transformArgument(token->arguments, "values");

    return new Engine::None(values);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Min* token)
{
    Engine::ValueObject* values = transformArgument(token->arguments, "values");

    return new Engine::Min(values);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Max* token)
{
    Engine::ValueObject* values = transformArgument(token->arguments, "values");

    return new Engine::Max(values);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Round* token)
{
    Engine::ValueObject* value = transformArgument(token->arguments, "value");
    Engine::ValueObject* step = transformArgument(token->arguments, "step");
    Engine::ValueObject* direction = transformArgument(token->arguments, "direction");

    return new Engine::Round(value, step, direction);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Absolute* token)
{
    Engine::ValueObject* value = transformArgument(token->arguments, "value");

    return new Engine::Absolute(value);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EmptyAudioSource* token)
{
    return new Engine::AudioSource();
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sine* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");
    Engine::ValueObject* frequency = transformArgument(token->arguments, "frequency");

    return new Engine::Sine(volume, pan, effects, frequency);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Square* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");
    Engine::ValueObject* frequency = transformArgument(token->arguments, "frequency");

    return new Engine::Square(volume, pan, effects, frequency);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Triangle* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");
    Engine::ValueObject* frequency = transformArgument(token->arguments, "frequency");

    return new Engine::Triangle(volume, pan, effects, frequency);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Saw* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");
    Engine::ValueObject* frequency = transformArgument(token->arguments, "frequency");

    return new Engine::Saw(volume, pan, effects, frequency);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Oscillator* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");
    Engine::ValueObject* frequency = transformArgument(token->arguments, "frequency");
    Engine::ValueObject* waveform = transformArgument(token->arguments, "waveform");

    return new Engine::CustomOscillator(volume, pan, effects, frequency, waveform);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Noise* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");

    return new Engine::Noise(volume, pan, effects);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sample* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");

    const Parser::String* str = static_cast<const Parser::String*>(token->arguments->get("file"));

    const Path* path = Path::beside(Path::formatPath(str->value), sourcePath);

    Engine::Resource* resource = new Engine::Resource(path, str->location);

    return new Engine::Sample(volume, pan, effects, resource);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Granulate* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");

    const Parser::String* str = static_cast<const Parser::String*>(token->arguments->get("sample"));

    const Path* path = Path::beside(Path::formatPath(str->value), sourcePath);

    Engine::Resource* resource = new Engine::Resource(path, str->location);

    Engine::ValueObject* grains = transformArgument(token->arguments, "grains");
    Engine::ValueObject* length = transformArgument(token->arguments, "length");
    Engine::ValueObject* shape = transformArgument(token->arguments, "shape");

    return new Engine::Granulate(volume, pan, effects, resource, grains, length, shape);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Group* token)
{
    Engine::ValueObject* volume = transformArgument(token->arguments, "volume");
    Engine::ValueObject* pan = transformArgument(token->arguments, "pan");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");
    Engine::ValueObject* sources = transformArgument(token->arguments, "sources");

    return new Engine::Group(volume, pan, effects, sources);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EmptyEffect* token)
{
    return new Engine::Effect();
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EffectGroup* token)
{
    Engine::ValueObject* mix = transformArgument(token->arguments, "mix");
    Engine::ValueObject* effects = transformArgument(token->arguments, "effects");

    return new Engine::EffectGroup(mix, effects);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Delay* token)
{
    Engine::ValueObject* mix = transformArgument(token->arguments, "mix");
    Engine::ValueObject* delay = transformArgument(token->arguments, "delay");
    Engine::ValueObject* feedback = transformArgument(token->arguments, "feedback");

    return new Engine::Delay(mix, delay, feedback);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Comb* token)
{
    Engine::ValueObject* mix = transformArgument(token->arguments, "mix");
    Engine::ValueObject* delay = transformArgument(token->arguments, "delay");
    Engine::ValueObject* feedback = transformArgument(token->arguments, "feedback");

    return new Engine::Comb(mix, delay, feedback);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::AllPass* token)
{
    Engine::ValueObject* mix = transformArgument(token->arguments, "mix");
    Engine::ValueObject* delay = transformArgument(token->arguments, "delay");
    Engine::ValueObject* feedback = transformArgument(token->arguments, "feedback");

    return new Engine::AllPass(mix, delay, feedback);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::LowPass* token)
{
    Engine::ValueObject* threshold = transformArgument(token->arguments, "threshold");

    return new Engine::LowPass(threshold);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Reverb* token)
{
    Engine::ValueObject* mix = transformArgument(token->arguments, "mix");
    Engine::ValueObject* length = transformArgument(token->arguments, "length");

    return new Engine::Reverb(mix, length);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::CallUser* token)
{
    for (Parser::InputDef* input : token->function->definition->inputs)
    {
        inputs[input] = transformArgument(token->arguments, input->str);
    }

    const Parser::Define* function = functions[token->function->definition];

    for (size_t i = 0; i < function->instructions.size() - 1; i++)
    {
        function->instructions[i]->transform(this);
    }

    Engine::ValueObject* value = function->instructions.back()->transform(this);

    return value;
}

Engine::ValueObject* TokenTransformer::transform(const Parser::AddAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueAdd(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::SubtractAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueSubtract(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::MultiplyAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueMultiply(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::DivideAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueDivide(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::PowerAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValuePower(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::EqualAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueEquals(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::LessAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueLess(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::GreaterAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueGreater(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::LessEqualAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueLessEqual(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::GreaterEqualAlias* token)
{
    Engine::ValueObject* a = token->a->transform(this);
    Engine::ValueObject* b = token->b->transform(this);

    return new Engine::ValueGreaterEqual(a, b);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Define* token)
{
    functions[token->function] = token;

    return nullptr;
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
