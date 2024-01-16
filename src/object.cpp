#include "../include/object.h"

Object::~Object() {}

Sync::Sync()
{
    utils = Utils::get();
}

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
