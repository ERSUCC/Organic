#include "../include/object.h"

Object::~Object() {}

Sync::Sync() :
    utils(Utils::get()) {}

void Sync::start(double time)
{
    if (!enabled)
    {
        startTime = time;
        repeatTime = time;

        enabled = true;

        finishStart();
    }
}

void Sync::repeat(double time)
{
    repeatTime = time;

    finishRepeat();
}

void Sync::stop()
{
    if (enabled)
    {
        enabled = false;

        finishStop();
    }
}

double Sync::syncLength()
{
    return 0;
}

void Sync::finishStart() {}
void Sync::finishRepeat() {}
void Sync::finishStop() {}

double ValueObject::getValue()
{
    return 0;
}

Variable::Variable(Object* object) :
    object(object), valueObject(dynamic_cast<ValueObject*>(object)) {}

double Variable::syncLength()
{
    return valueObject ? valueObject->syncLength() : 0;
}

double Variable::getValue()
{
    if (valueObject)
    {
        if (!valueObject->enabled)
        {
            stop();
        }

        return valueObject->getValue();
    }

    return 0;
}

void Variable::finishStart()
{
    if (valueObject)
    {
        valueObject->start(startTime);
    }
}

void Variable::finishStop()
{
    if (valueObject)
    {
        valueObject->stop();
    }
}
