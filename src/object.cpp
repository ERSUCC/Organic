#include "../include/object.h"

Object::Object()
{
    utils = Utils::get();
}

Object::~Object() {}

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

void Variable::finishStart()
{
    value->start(startTime);
}

void Variable::finishStop()
{
    value->stop();
}
