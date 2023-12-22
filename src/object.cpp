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

        if (parent)
        {
            parent->childStart(this);
        }
    }
}

void Sync::stop()
{
    if (enabled)
    {
        enabled = false;

        finishStop();

        if (parent)
        {
            parent->childStop(this);
        }
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

void Sync::childStart(Sync* child) {}
void Sync::childStop(Sync* child) {}

double ValueObject::getValue(bool force)
{
    if (force || enabled)
    {
        return getValueUnchecked();
    }

    return 0;
}

double Variable::syncLength()
{
    return value->syncLength();
}

void Variable::finishStart()
{
    value->start();
}

void Variable::finishStop()
{
    value->stop();
}

void Variable::childStart(Sync* child)
{
    if (!enabled)
    {
        enabled = true;
    }
}

void Variable::childStop(Sync* child)
{
    if (enabled)
    {
        enabled = false;
    }
}

double Variable::getValueUnchecked()
{
    return value->getValue();
}

double ValueCombination::syncLength()
{
    return value1->syncLength();
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
