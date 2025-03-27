#include "../include/controller.h"

ValueByte::ValueByte(const unsigned char value) :
    value(value) {}

ValueInt::ValueInt(const unsigned int value) :
    value(value) {}

ValueCombination::ValueCombination(ValueObject* value1, ValueObject* value2) :
    value1(value1), value2(value2) {}

double ValueCombination::syncLength() const
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

All::All(ValueObject* values) :
    values(values) {}

double All::getValue()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (object->getValue() == 0)
        {
            return 0;
        }
    }

    return 1;
}

void All::init()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

Any::Any(ValueObject* values) :
    values(values) {}

double Any::getValue()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (object->getValue() != 0)
        {
            return 1;
        }
    }

    return 0;
}

void Any::init()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

None::None(ValueObject* values) :
    values(values) {}

double None::getValue()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (object->getValue() != 0)
        {
            return 0;
        }
    }

    return 1;
}

void None::init()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

Min::Min(ValueObject* values) :
    values(values) {}

double Min::getValue()
{
    const std::vector<ValueObject*>& objects = values->getLeafAs<List>()->objects;

    if (objects.empty())
    {
        return 0;
    }

    double min = utils->infinity;

    for (ValueObject* object : objects)
    {
        const double value = object->getValue();

        if (value < min)
        {
            min = value;
        }
    }

    return min;
}

void Min::init()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

Max::Max(ValueObject* values) :
    values(values) {}

double Max::getValue()
{
    const std::vector<ValueObject*>& objects = values->getLeafAs<List>()->objects;

    if (objects.empty())
    {
        return 0;
    }

    double max = -utils->infinity;

    for (ValueObject* object : objects)
    {
        double value = object->getValue();

        if (value > max)
        {
            max = value;
        }
    }

    return max;
}

void Max::init()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

Round::Round(ValueObject* value, ValueObject* step, ValueObject* direction) :
    value(value), step(step), direction(direction) {}

double Round::syncLength() const
{
    return value->syncLength();
}

double Round::getValue()
{
    if (utils->time - startTime >= syncLength())
    {
        stop();
    }

    const double val = value->getValue();
    double st = step->getValue();

    if (st == 0)
    {
        st = 1;
    }

    switch (direction->getLeafAs<ValueByte>()->value)
    {
        case DirectionEnum::Nearest:
            return round(val / st) * st;

            break;

        case DirectionEnum::Up:
            return ceil(val / st) * st;

            break;

        case DirectionEnum::Down:
            return floor(val / st) * st;

            break;
    }

    return 0;
}

void Round::init()
{
    value->start(startTime);
    step->start(startTime);
    direction->start(startTime);
}

Sequence::Sequence(ValueObject* controllers, ValueObject* order) :
    controllers(controllers), order(order) {}

double Sequence::syncLength() const
{
    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    if (objects.empty())
    {
        return 0;
    }

    double length = 0;

    if (order->getLeafAs<ValueByte>()->value == OrderEnum::PingPong)
    {
        length += objects[0]->syncLength();

        for (unsigned int i = 1; i < objects.size() - 1; i++)
        {
            length += objects[i]->syncLength() * 2;
        }

        if (objects.size() > 1)
        {
            length += objects.back()->syncLength();
        }
    }

    else
    {
        for (ValueObject* controller : objects)
        {
            length += controller->syncLength();
        }
    }

    return length;
}

double Sequence::getValue()
{
    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    if (objects.empty())
    {
        return 0;
    }

    if (!objects[current]->enabled)
    {
        last = current;

        if (++switches <= max_switches)
        {
            repeat(repeatTime + objects[current]->syncLength());
        }

        else
        {
            stop();
        }
    }

    return objects[current]->getValue();
}

void Sequence::init()
{
    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    if (objects.empty())
    {
        return;
    }

    switches = 0;

    chosen.clear();

    udist = std::uniform_int_distribution<>(0, objects.size() - 1);

    const unsigned char orderNum = order->getLeafAs<ValueByte>()->value;

    if (orderNum == OrderEnum::PingPong)
    {
        max_switches = objects.size() * 2 - 2;
    }

    else
    {
        max_switches = objects.size() - 1;
    }

    if (orderNum == OrderEnum::Backwards)
    {
        current = objects.size() - 1;
    }

    else if (orderNum == OrderEnum::Random)
    {
        current = udist(utils->rng);

        if (current == last)
        {
            current = (current + 1) % objects.size();
        }

        chosen.insert(current);
    }

    else
    {
        current = 0;
    }

    objects[current]->start(startTime);
}

void Sequence::reinit()
{
    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    if (objects.empty())
    {
        return;
    }

    switch (order->getLeafAs<ValueByte>()->value)
    {
        case OrderEnum::Forwards:
            current = (current + 1) % objects.size();

            break;

        case OrderEnum::Backwards:
        {
            current -= 1;

            if (current < 0)
            {
                current = objects.size() - 1;
            }

            break;
        }

        case OrderEnum::PingPong:
        {
            if ((direction == -1 && current <= 0) || current >= objects.size() - 1)
            {
                direction *= -1;
            }

            current += direction;

            break;
        }

        case OrderEnum::Random:
        {
            if (chosen.size() < objects.size())
            {
                current = udist(utils->rng);

                while (chosen.count(current))
                {
                    current = (current + 1) % objects.size();
                }

                chosen.insert(current);
            }

            break;
        }
    }

    objects[current]->start(repeatTime);
}

Repeat::Repeat(ValueObject* value, ValueObject* repeats) :
    value(value), repeats(repeats) {}

double Repeat::syncLength() const
{
    if (repeats->getValue() == 0)
    {
        return utils->infinity;
    }

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

double Hold::syncLength() const
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

double Sweep::syncLength() const
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

double LFO::syncLength() const
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

double Random::syncLength() const
{
    return length->getValue();
}

double Random::getValue()
{
    if (utils->time - startTime >= syncLength())
    {
        stop();
    }

    switch (type->getLeafAs<ValueByte>()->value)
    {
        case TypeEnum::Step:
            return current;

        case TypeEnum::Linear:
            return current + (next - current) * (utils->time - startTime) / syncLength();
    }

    return 0;
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

double Limit::syncLength() const
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

double Trigger::syncLength() const
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
