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
    if (!enabled)
    {
        startTime = time;

        enabled = true;

        finishStart();
    }
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

double Sync::getStartTime()
{
    return startTime;
}

void Sync::finishStart() {}
void Sync::finishStop() {}

double ValueObject::getValue()
{
    return 0;
}

double Variable::syncLength()
{
    return value->syncLength();
}

double Variable::getValue()
{
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

double ValueCombination::syncLength()
{
    return value1->syncLength();
}

void ValueCombination::finishStart()
{
    value1->start(startTime);
    value2->start(startTime);
}

void ValueCombination::finishStop()
{
    value1->stop();
    value2->stop();
}

double ValueAdd::getValue()
{
    return value1->getValue() + value2->getValue();
}

double ValueSubtract::getValue()
{
    return value1->getValue() - value2->getValue();
}

double ValueMultiply::getValue()
{
    return value1->getValue() * value2->getValue();
}

double ValueDivide::getValue()
{
    return value1->getValue() / value2->getValue();
}
