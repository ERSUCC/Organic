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

void Sync::stop(const double time)
{
    if (enabled)
    {
        stopTime = time;
        enabled = false;
    }
}

void Sync::init() {}
void Sync::reinit() {}

void Defaults::deinit()
{
    for (const std::pair<std::type_index, ValueObject*>& pair : objects)
    {
        delete pair.second;
    }
}

ValueObject::~ValueObject() {}

double ValueObject::getValue() const
{
    return 0;
}

ValueObject* ValueObject::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return this;
}

void ValueObject::update() {}

List::List(const std::vector<ValueObject*>& objects) :
    objects(objects) {}

List::~List()
{
    for (const ValueObject* object : objects)
    {
        delete object;
    }
}

Variable::Variable(ValueObject* value) :
    value(value) {}

double Variable::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return value->getValue();
}

ValueObject* Variable::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return value->getLeaf();
}

void Variable::update()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
}

void Variable::init()
{
    value->start(startTime);
}

Lambda::Lambda(const std::vector<Variable*>& inputs, ValueObject* value) :
    inputs(inputs), value(value) {}

Lambda::Lambda() :
    inputs({}), value(new ValueObject()) {}

Lambda::~Lambda()
{
    for (Variable* input : inputs)
    {
        delete input;
    }

    delete value;
}

double Lambda::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return value->getValue();
}

void Lambda::update()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
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
