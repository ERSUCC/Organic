#include "../include/object.h"

Object::~Object() {}

Sync::Sync()
{
    utils = Utils::get();
}

void Sync::start()
{
    start(utils->time);
}

void Sync::start(double time)
{
    startTime = time;

    enabled = true;

    finishStart();
}

void Sync::stop()
{
    enabled = false;

    finishStop();
}

double Sync::syncLength()
{
    return 0;
}

void Sync::finishStart() {}
void Sync::finishStop() {}

double ValueObject::getValue()
{
    if (enabled)
    {
        return getValueUnchecked();
    }

    return 0;
}

double Variable::getValueUnchecked()
{
    return value->getValue();
}

void Variable::finishStart()
{
    value->start();
}

void Variable::finishStop()
{
    value->stop();
}