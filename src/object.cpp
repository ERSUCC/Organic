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

void ValueObject::update()
{
    if (lastUpdate != utils->time)
    {
        lastUpdate = utils->time;

        compute();
    }
}

void ValueObject::compute() {}

String::String(const std::string& value) :
    value(value) {}

String::String() :
    value("") {}

List::List(const std::vector<ValueObject*>& objects) :
    objects(objects) {}

List::~List()
{
    for (const ValueObject* object : objects)
    {
        delete object;
    }
}

MultiList::MultiList(ValueObject* object) :
    object(object) {}

MultiList::~MultiList()
{
    delete object;
}

ValueObject* MultiList::getLeaf()
{
    return object->getLeaf();
}

void MultiList::init()
{
    object->start(startTime);

    currentList = object->getLeafAs<List>();

    for (ValueObject* object : currentList->objects)
    {
        object->start(startTime);
    }
}

void MultiList::compute()
{
    object->update();

    List* list = object->getLeafAs<List>();

    if (list != currentList)
    {
        for (ValueObject* object : list->objects)
        {
            object->start(list->getStartTime());
        }

        currentList = list;
    }

    for (ValueObject* object : currentList->objects)
    {
        object->update();
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

void Variable::init()
{
    value->start(startTime);
}

void Variable::compute()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
}

Lambda::Lambda(const std::unordered_map<std::string, Variable*>& inputs, ValueObject* value) :
    inputs(inputs), value(value) {}

Lambda::~Lambda()
{
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

void Lambda::setInput(const std::string& name, ValueObject* value)
{
    inputs.at(name)->value = value;
}

void Lambda::init()
{
    value->start(startTime);
}

void Lambda::compute()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
}
