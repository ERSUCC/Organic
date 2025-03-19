#include "../include/object.h"

Sync::Sync() :
    utils(Utils::get()) {}

void Sync::start(double time)
{
    if (!enabled)
    {
        startTime = time;
        repeatTime = time;

        enabled = true;

        init();
    }
}

void Sync::repeat(double time)
{
    repeatTime = time;

    reinit();
}

void Sync::stop()
{
    enabled = false;
}

double Sync::syncLength() const
{
    return utils->infinity;
}

void Sync::init() {}
void Sync::reinit() {}

double ValueObject::getValue()
{
    return 0;
}

ValueObject* ValueObject::expandVariable()
{
    return this;
}

List* ValueObject::getList()
{
    return nullptr;
}

Lambda* ValueObject::getLambda()
{
    return nullptr;
}

Resource* ValueObject::getResource()
{
    return nullptr;
}

Default::Default() :
    list(new List({})), lambda(new Lambda({}, this)), resource(new Resource(nullptr, 0, utils->sampleRate, utils->channels)) {}

Default* Default::get()
{
    static Default* instance;

    if (!instance)
    {
        instance = new Default();
    }

    return instance;
}

List* Default::getList()
{
    return list;
}

Lambda* Default::getLambda()
{
    return lambda;
}

Resource* Default::getResource()
{
    return resource;
}

List::List(const std::vector<ValueObject*> objects) :
    objects(objects) {}

List* List::getList()
{
    return this;
}

Variable::Variable(ValueObject* value) :
    value(value) {}

double Variable::syncLength() const
{
    return value->syncLength();
}

double Variable::getValue()
{
    if (!value->enabled)
    {
        stop();
    }

    return value->getValue();
}

ValueObject* Variable::expandVariable()
{
    return value->expandVariable();
}

List* Variable::getList()
{
    return value->getList();
}

Lambda* Variable::getLambda()
{
    return value->getLambda();
}

Resource* Variable::getResource()
{
    return value->getResource();
}

void Variable::init()
{
    value->start(startTime);
}

Lambda::Lambda(const std::vector<Variable*> inputs, ValueObject* value) :
    inputs(inputs), value(value) {}

double Lambda::getValue()
{
    return value->getValue();
}

Lambda* Lambda::getLambda()
{
    return this;
}

void Lambda::setInputs(const std::vector<ValueObject*>& values)
{
    for (unsigned int i = 0; i < inputs.size(); i++)
    {
        inputs[i]->value = values[i];
    }
}

void Lambda::init()
{
    for (Variable* input : inputs)
    {
        input->start(startTime);
    }

    value->start(startTime);
}

Resource::Resource(double* samples, const unsigned int length, const unsigned int sampleRate, const unsigned int channels) :
    samples(samples), length(length), sampleRate(sampleRate), channels(channels) {}

Resource::~Resource()
{
    free(samples);
}

Resource* Resource::getResource()
{
    return this;
}

double Time::getValue()
{
    return utils->time;
}
