#include "../include/controller.h"

ValueCombination::ValueCombination(ValueObject* value1, ValueObject* value2) :
    value1(value1), value2(value2) {}

double ValueCombination::syncLength()
{
    return value1->syncLength();
}

double ValueCombination::getValue()
{
    if (!value1->enabled || !value2->enabled)
    {
        stop();
    }

    return getValueInternal();
}

void ValueCombination::init()
{
    value1->start(startTime);
    value2->start(startTime);
}

ValueAdd::ValueAdd(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueAdd::getValueInternal()
{
    return value1->getValue() + value2->getValue();
}

ValueSubtract::ValueSubtract(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueSubtract::getValueInternal()
{
    return value1->getValue() - value2->getValue();
}

ValueMultiply::ValueMultiply(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueMultiply::getValueInternal()
{
    return value1->getValue() * value2->getValue();
}

ValueDivide::ValueDivide(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueDivide::getValueInternal()
{
    return value1->getValue() / value2->getValue();
}

ValuePower::ValuePower(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValuePower::getValueInternal()
{
    return pow(value1->getValue(), value2->getValue());
}

ValueEquals::ValueEquals(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueEquals::getValueInternal()
{
    if (value1->getValue() == value2->getValue())
    {
        return 1;
    }

    return 0;
}

ValueLess::ValueLess(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueLess::getValueInternal()
{
    if (value1->getValue() < value2->getValue())
    {
        return 1;
    }

    return 0;
}

ValueGreater::ValueGreater(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueGreater::getValueInternal()
{
    if (value1->getValue() > value2->getValue())
    {
        return 1;
    }

    return 0;
}

ValueLessEqual::ValueLessEqual(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueLessEqual::getValueInternal()
{
    if (value1->getValue() <= value2->getValue())
    {
        return 1;
    }

    return 0;
}

ValueGreaterEqual::ValueGreaterEqual(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueGreaterEqual::getValueInternal()
{
    if (value1->getValue() >= value2->getValue())
    {
        return 1;
    }

    return 0;
}

Sequence::Sequence(ValueObject* controllers, ValueObject* order) :
    controllers(controllers), order(order) {}

double Sequence::syncLength()
{
    double length = 0;

    if (order->getValue() == OrderEnum::PingPong)
    {
        length += controllers->getList()->objects[0]->syncLength();

        for (unsigned int i = 1; i < controllers->getList()->objects.size() - 1; i++)
        {
            length += controllers->getList()->objects[i]->syncLength() * 2;
        }

        if (controllers->getList()->objects.size() > 1)
        {
            length += controllers->getList()->objects.back()->syncLength();
        }
    }

    else
    {
        for (ValueObject* controller : controllers->getList()->objects)
        {
            length += controller->syncLength();
        }
    }

    return length;
}

double Sequence::getValue()
{
    if (!controllers->getList()->objects[current]->enabled)
    {
        last = current;

        if (++switches <= max_switches)
        {
            repeat(repeatTime + controllers->getList()->objects[current]->syncLength());
        }

        else
        {
            stop();
        }
    }

    return controllers->getList()->objects[current]->getValue();
}

void Sequence::init()
{
    switches = 0;

    chosen.clear();

    udist = std::uniform_int_distribution<>(0, controllers->getList()->objects.size() - 1);

    const unsigned int orderNum = (unsigned int)order->getValue();

    if (orderNum == OrderEnum::PingPong)
    {
        max_switches = controllers->getList()->objects.size() * 2 - 2;
    }

    else
    {
        max_switches = controllers->getList()->objects.size() - 1;
    }

    if (orderNum == OrderEnum::Backwards)
    {
        current = controllers->getList()->objects.size() - 1;
    }

    else if (orderNum == OrderEnum::Random)
    {
        current = udist(utils->rng);

        if (current == last)
        {
            current = (current + 1) % controllers->getList()->objects.size();
        }

        chosen.insert(current);
    }

    else
    {
        current = 0;
    }

    controllers->getList()->objects[current]->start(startTime);
}

void Sequence::reinit()
{
    switch ((unsigned int)order->getValue())
    {
        case OrderEnum::Forwards:
            current = (current + 1) % controllers->getList()->objects.size();

            break;

        case OrderEnum::Backwards:
        {
            current -= 1;

            if (current < 0)
            {
                current = controllers->getList()->objects.size() - 1;
            }

            break;
        }

        case OrderEnum::PingPong:
        {
            if ((direction == -1 && current <= 0) || current >= controllers->getList()->objects.size() - 1)
            {
                direction *= -1;
            }

            current += direction;

            break;
        }

        case OrderEnum::Random:
        {
            if (chosen.size() < controllers->getList()->objects.size())
            {
                current = udist(utils->rng);

                while (chosen.count(current))
                {
                    current = (current + 1) % controllers->getList()->objects.size();
                }

                chosen.insert(current);
            }

            break;
        }

        default:
            // should this be an error?

            break;
    }

    controllers->getList()->objects[current]->start(repeatTime);
}

Repeat::Repeat(ValueObject* value, ValueObject* repeats) :
    value(value), repeats(repeats) {}

double Repeat::syncLength()
{
    return value->syncLength() * repeats->getValue();
}

double Repeat::getValue()
{
    if (!value->enabled)
    {
        if (repeats->getValue() == 0 || ++times < repeats->getValue())
        {
            repeat(repeatTime + value->syncLength());
        }

        else
        {
            stop();
        }
    }

    return value->getValue();
}

void Repeat::init()
{
    value->start(startTime);
    repeats->start(startTime);

    times = 0;
}

void Repeat::reinit()
{
    value->start(repeatTime);
}

Value::Value(double value) :
    value(value) {}

double Value::getValue()
{
    return value;
}

Hold::Hold(ValueObject* value, ValueObject* length) :
    value(value), length(length) {}

double Hold::syncLength()
{
    return length->getValue();
}

double Hold::getValue()
{
    if (utils->time - startTime >= syncLength())
    {
        stop();
    }

    return value->getValue();
}

void Hold::init()
{
    value->start(startTime);
    length->start(startTime);
}

Sweep::Sweep(ValueObject* from, ValueObject* to, ValueObject* length) :
    from(from), to(to), length(length) {}

double Sweep::syncLength()
{
    return length->getValue();
}

double Sweep::getValue()
{
    if (utils->time - startTime >= syncLength())
    {
        stop();

        return to->getValue();
    }

    return from->getValue() + (to->getValue() - from->getValue()) * (utils->time - startTime) / syncLength();
}

void Sweep::init()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

LFO::LFO(ValueObject* from, ValueObject* to, ValueObject* length) :
    from(from), to(to), length(length) {}

double LFO::syncLength()
{
    return length->getValue();
}

double LFO::getValue()
{
    if (utils->time - startTime >= syncLength())
    {
        stop();

        return from->getValue();
    }

    return from->getValue() + (to->getValue() - from->getValue()) * (-cos(utils->twoPi * (utils->time - startTime) / syncLength()) / 2 + 0.5);
}

void LFO::init()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

Random::Random(ValueObject* from, ValueObject* to, ValueObject* length, ValueObject* type) :
    from(from), to(to), length(length), type(type) {}

double Random::syncLength()
{
    return length->getValue();
}

double Random::getValue()
{
    if (utils->time - startTime >= syncLength())
    {
        stop();
    }

    switch ((unsigned int)type->getValue())
    {
        case TypeEnum::Step:
            return current;

        case TypeEnum::Linear:
            return current + (next - current) * (utils->time - startTime) / syncLength();

        default:
            return 0; // should this be an error?
    }
}

void Random::init()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);

    std::uniform_real_distribution<> udist(from->getValue(), to->getValue());

    if (first)
    {
        current = udist(utils->rng);
    }

    else
    {
        current = next;
    }
    
    next = udist(utils->rng);

    first = false;
}

Limit::Limit(ValueObject* value, ValueObject* min, ValueObject* max) :
    value(value), min(min), max(max) {}

double Limit::syncLength()
{
    return value->syncLength();
}

double Limit::getValue()
{
    if (!value->enabled)
    {
        stop();
    }

    const double valueValue = value->getValue();
    const double minValue = min->getValue();
    const double maxValue = max->getValue();

    if (valueValue < minValue)
    {
        return minValue;
    }

    if (valueValue > maxValue)
    {
        return maxValue;
    }

    return valueValue;
}

void Limit::init()
{
    value->start(startTime);
    min->start(startTime);
    max->start(startTime);
}

Trigger::Trigger(ValueObject* condition, ValueObject* value) :
    condition(condition), value(value) {}

double Trigger::syncLength()
{
    return value->syncLength();
}

double Trigger::getValue()
{
    if (triggered && !value->enabled && condition->getValue() == 0)
    {
        triggered = false;
    }

    if (!triggered && condition->getValue() != 0)
    {
        triggered = true;

        value->start(utils->time);
    }

    if (value->enabled)
    {
        return value->getValue();
    }

    return 0;
}

void Trigger::init()
{
    condition->start(startTime);
}

If::If(ValueObject* condition, ValueObject* trueValue, ValueObject* falseValue) :
    condition(condition), trueValue(trueValue), falseValue(falseValue) {}

double If::syncLength()
{
    return utils->infinity;
}

double If::getValue()
{
    if (condition->getValue() == 0)
    {
        return falseValue->getValue();
    }

    return trueValue->getValue();
}

void If::init()
{
    condition->start(startTime);
    trueValue->start(startTime);
    falseValue->start(startTime);
}
