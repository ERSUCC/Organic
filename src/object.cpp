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

double Sync::syncLength()
{
    return 0;
}

void Sync::init() {}
void Sync::reinit() {}

double ValueObject::getValue()
{
    return 0;
}

List<ValueObject>* ValueObject::getList()
{
    return new List<ValueObject>({ this });
}

template <typename T> List<T>::List(const std::vector<T*> objects) :
    objects(objects) {}

template <typename T> List<ValueObject>* List<T>::getList()
{
    return this;
}

Variable::Variable(ValueObject* value) :
    value(value) {}

double Variable::syncLength()
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

List<ValueObject>* Variable::getList()
{
    return value->getList();
}

void Variable::init()
{
    value->start(startTime);
}
