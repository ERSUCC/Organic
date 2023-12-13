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

void Variable::finishStart()
{
    value->start();
}

void Variable::finishStop()
{
    value->stop();
}

double Variable::getValueUnchecked()
{
    return value->getValue();
}

void ValueCombination::finishStart()
{
    value1->start();
    value2->start();
}

void ValueCombination::finishStop()
{
    value1->stop();
    value2->stop();
}

double ValueAdd::getValueUnchecked()
{
    return value1->getValue() + value2->getValue();
}

double ValueSubtract::getValueUnchecked()
{
    return value1->getValue() - value2->getValue();
}

double ValueMultiply::getValueUnchecked()
{
    return value1->getValue() * value2->getValue();
}

double ValueDivide::getValueUnchecked()
{
    return value1->getValue() / value2->getValue();
}
