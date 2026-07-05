#include "../include/object.h"

using namespace Engine;

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

ValueObject::~ValueObject() {}

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

List::~List()
{
    for (ValueObject* object : objects)
    {
        delete object;
    }
}

void List::init()
{
    for (ValueObject* object : objects)
    {
        object->start(startTime);
    }
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

Lambda::~Lambda()
{
    for (Variable* input : inputs)
    {
        delete input;
    }

    delete value;
}

double Lambda::getValue()
{
    return value->getValue();
}

void Lambda::setInputs(const std::vector<ValueObject*>& values)
{
    for (size_t i = 0; i < inputs.size(); i++)
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

double Time::getValue()
{
    return utils->time;
}
