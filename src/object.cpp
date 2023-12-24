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

ValueCombination::ValueCombination(ValueObject* value1, ValueObject* value2) :
    value1(value1), value2(value2) {}

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

ValueAdd::ValueAdd(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueAdd::getValue()
{
    return value1->getValue() + value2->getValue();
}

ValueSubtract::ValueSubtract(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueSubtract::getValue()
{
    return value1->getValue() - value2->getValue();
}

ValueMultiply::ValueMultiply(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueMultiply::getValue()
{
    return value1->getValue() * value2->getValue();
}

ValueDivide::ValueDivide(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueDivide::getValue()
{
    return value1->getValue() / value2->getValue();
}
