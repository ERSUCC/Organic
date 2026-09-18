#include "../include/transform.h"

#define ARG(name) transformArgument(token->arguments, name)
#define FILL_ARG(name, inputs) fillArgument(token->arguments, name, inputs)

LambdaContext::LambdaContext(LambdaContext* parent, const std::vector<std::string>& inputs, std::vector<Engine::ValueObject*>& variables) :
    parent(parent)
{
    for (const std::string& input : inputs)
    {
        fillables[input] = new Engine::Variable(Engine::Defaults::get<Engine::ValueObject>());

        variables.push_back(fillables[input]);
    }
}

Engine::Variable* LambdaContext::findFillable(const std::string& name)
{
    if (fillables.count(name))
    {
        return fillables[name];
    }

    if (parent)
    {
        return parent->findFillable(name);
    }

    return nullptr;
}

TokenTransformer::TokenTransformer(const Path& sourcePath) :
    sourcePath(sourcePath) {}

TokenTransformer::~TokenTransformer()
{
    while (context)
    {
        const LambdaContext* current = context;

        context = context->parent;

        delete current;
    }
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Fillable* token)
{
    return new Engine::Variable(context->findFillable(token->name));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Value* token)
{
    return new Engine::Value(token->value);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Constant* token)
{
    return new Engine::ValueChar(token->value);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Boolean* token)
{
    return new Engine::Value(token->value ? 1 : 0);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::VariableDef* token)
{
    setVariable(token, token->value->transform(this));

    return nullptr;
}

Engine::ValueObject* TokenTransformer::transform(const Parser::VariableRef* token)
{
    return new Engine::Variable(currentVariables[token->definition]);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::InputRef* token)
{
    return new Engine::Variable(currentVariables[token->definition]);
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

Engine::ValueObject* TokenTransformer::transform(const Parser::Negate* token)
{
    return new Engine::ValueNegate(token->value->transform(this));
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
    return new Engine::If(ARG("condition"), ARG("is-true"), ARG("is-false"));
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

Engine::ValueObject* TokenTransformer::transform(const Parser::Modulo* token)
{
    return new Engine::Modulo(ARG("value"), ARG("divisor"));
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
    return new Engine::CustomOscillator(ARG("volume"), ARG("pan"), ARG("effects"), ARG("frequency"), FILL_ARG("waveform", { "phase" }));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Noise* token)
{
    return new Engine::Noise(ARG("volume"), ARG("pan"), ARG("effects"));
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Sample* token)
{
    const Parser::Argument* file = token->arguments->findArgument("file");
    const Parser::String* str = dynamic_cast<const Parser::String*>(file->value.get());

    const Path path = Path::beside(Path::formatPath(str->str), sourcePath);

    Engine::Resource* resource = new Engine::Resource(path, file->location);

    return new Engine::Sample(ARG("volume"), ARG("pan"), ARG("effects"), resource);
}

Engine::ValueObject* TokenTransformer::transform(const Parser::Granulate* token)
{
    const Parser::Argument* file = token->arguments->findArgument("sample");
    const Parser::String* str = dynamic_cast<const Parser::String*>(file->value.get());

    const Path path = Path::beside(Path::formatPath(str->str), sourcePath);

    Engine::Resource* resource = new Engine::Resource(path, file->location);

    return new Engine::Granulate(ARG("volume"), ARG("pan"), ARG("effects"), resource, ARG("grains"), ARG("length"), FILL_ARG("shape", { "position" }));
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
    for (const Parser::InputDef* input : token->function->inputs)
    {
        setVariable(input, transformArgument(token->arguments, input->string()));
    }

    for (size_t i = 0; i < token->function->program->instructions.size() - 1; i++)
    {
        delete token->function->program->instructions[i]->transform(this);
    }

    return token->function->program->instructions.back()->transform(this);
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

Engine::Program* TokenTransformer::transform(const Parser::Program* token)
{
    std::vector<Engine::ValueObject*> sources;

    for (const Parser::Token* instruction : token->instructions)
    {
        Engine::ValueObject* object = instruction->transform(this);

        if (instruction->type()->baseType() == Parser::TypeConstant::AudioSource)
        {
            sources.push_back(object);
        }

        else
        {
            delete object;
        }
    }

    return new Engine::Program(allVariables, sources);
}

Engine::ValueObject* TokenTransformer::transformArgument(const Parser::ArgumentList* arguments, const std::string& name)
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

Engine::Lambda* TokenTransformer::fillArgument(const Parser::ArgumentList* arguments, const std::string& name, const std::vector<std::string>& inputs)
{
    context = new LambdaContext(context, inputs, allVariables);

    Engine::Lambda* argument = new Engine::Lambda(context->fillables, transformArgument(arguments, name));

    const LambdaContext* current = context;

    context = context->parent;

    delete current;

    return argument;
}

void TokenTransformer::setVariable(const Parser::Identifier* name, Engine::ValueObject* value)
{
    currentVariables[name] = value;

    allVariables.push_back(value);
}
