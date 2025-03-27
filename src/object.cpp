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

ValueObject* ValueObject::getLeaf()
{
    return this;
}

List::List(const std::vector<ValueObject*> objects) :
    objects(objects) {}

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

ValueObject* Variable::getLeaf()
{
    return value->getLeaf();
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

double Time::getValue()
{
    return utils->time;
}
