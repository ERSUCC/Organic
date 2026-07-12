#include "../include/controller.h"

using namespace Engine;

Value::Value(const double value) :
    value(value) {}

double Value::getValue()
{
    return value;
}

ValueByte::ValueByte(const unsigned char value) :
    value(value) {}

ValueNegate::ValueNegate(ValueObject* value) :
    value(value) {}

double ValueNegate::getValue()
{
    return -value->getValue();
}

ValueCombination::ValueCombination(ValueObject* value1, ValueObject* value2) :
    value1(value1), value2(value2) {}

ValueCombination::~ValueCombination()
{
    delete value1;
    delete value2;
}

double ValueCombination::syncLength() const
{
    return fmin(value1->syncLength(), value2->syncLength());
}

double ValueCombination::getValue()
{
    const double val1 = value1->getValue();
    const double val2 = value2->getValue();

    if (value1->enabled && value2->enabled)
    {
        return getValueInternal(val1, val2);
    }

    stop();

    return 0;
}

void ValueCombination::init()
{
    value1->start(startTime);
    value2->start(startTime);
}

ValueAdd::ValueAdd(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueAdd::getValueInternal(const double value1, const double value2) const
{
    return value1 + value2;
}

ValueSubtract::ValueSubtract(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueSubtract::getValueInternal(const double value1, const double value2) const
{
    return value1 - value2;
}

ValueMultiply::ValueMultiply(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueMultiply::getValueInternal(const double value1, const double value2) const
{
    return value1 * value2;
}

ValueDivide::ValueDivide(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueDivide::getValueInternal(const double value1, const double value2) const
{
    return value1 / value2;
}

ValuePower::ValuePower(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValuePower::getValueInternal(const double value1, const double value2) const
{
    return pow(value1, value2);
}

ValueEquals::ValueEquals(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueEquals::getValueInternal(const double value1, const double value2) const
{
    return value1 == value2;
}

ValueLess::ValueLess(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueLess::getValueInternal(const double value1, const double value2) const
{
    return value1 < value2;
}

ValueGreater::ValueGreater(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueGreater::getValueInternal(const double value1, const double value2) const
{
    return value1 > value2;
}

ValueLessEqual::ValueLessEqual(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueLessEqual::getValueInternal(const double value1, const double value2) const
{
    return value1 <= value2;
}

ValueGreaterEqual::ValueGreaterEqual(ValueObject* value1, ValueObject* value2) :
    ValueCombination(value1, value2) {}

double ValueGreaterEqual::getValueInternal(const double value1, const double value2) const
{
    return value1 >= value2;
}

All::All(ValueObject* values) :
    values(values) {}

All::~All()
{
    delete values;
}

double All::getValue()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        const double val = object->getValue();

        if (!object->enabled)
        {
            stop();

            return 0;
        }

        if (val == 0)
        {
            return 0;
        }
    }

    return 1;
}

void All::init()
{
    values->start(startTime);
}

Any::Any(ValueObject* values) :
    values(values) {}

Any::~Any()
{
    delete values;
}

double Any::getValue()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (object->getValue() != 0 && object->enabled)
        {
            return 1;
        }

        if (!object->enabled)
        {
            stop();

            return 0;
        }
    }

    return 0;
}

void Any::init()
{
    values->start(startTime);
}

None::None(ValueObject* values) :
    values(values) {}

None::~None()
{
    delete values;
}

double None::getValue()
{
    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (object->getValue() != 0 && object->enabled)
        {
            return 0;
        }

        if (!object->enabled)
        {
            stop();

            return 0;
        }
    }

    return 1;
}

void None::init()
{
    values->start(startTime);
}

Min::Min(ValueObject* values) :
    values(values) {}

Min::~Min()
{
    delete values;
}

double Min::getValue()
{
    const std::vector<ValueObject*>& objects = values->getLeafAs<List>()->objects;

    double min = utils->infinity;

    for (ValueObject* object : objects)
    {
        const double value = object->getValue();

        if (!object->enabled)
        {
            stop();

            return 0;
        }

        if (value < min)
        {
            min = value;
        }
    }

    return min;
}

void Min::init()
{
    values->start(startTime);
}

Max::Max(ValueObject* values) :
    values(values) {}

Max::~Max()
{
    delete values;
}

double Max::getValue()
{
    const std::vector<ValueObject*>& objects = values->getLeafAs<List>()->objects;

    double max = -utils->infinity;

    for (ValueObject* object : objects)
    {
        const double value = object->getValue();

        if (!object->enabled)
        {
            stop();

            return 0;
        }

        if (value > max)
        {
            max = value;
        }
    }

    return max;
}

void Max::init()
{
    values->start(startTime);
}

Round::Round(ValueObject* value, ValueObject* step, ValueObject* direction) :
    value(value), step(step), direction(direction) {}

Round::~Round()
{
    delete value;
    delete step;
    delete direction;
}

double Round::syncLength() const
{
    return value->syncLength();
}

double Round::getValue()
{
    const double val = value->getValue();

    if (!value->enabled)
    {
        stop();

        return 0;
    }

    double st = step->getValue();

    if (st == 0)
    {
        return val;
    }

    switch (direction->getLeafAs<ValueByte>()->value)
    {
        case Constants::Round::Nearest:
            return round(val / st) * st;

        case Constants::Round::Up:
            return ceil(val / st) * st;

        case Constants::Round::Down:
            return floor(val / st) * st;
    }

    return 0;
}

void Round::init()
{
    value->start(startTime);
    step->start(startTime);
    direction->start(startTime);
}

Absolute::Absolute(ValueObject* value) :
    value(value) {}

Absolute::~Absolute()
{
    delete value;
}

double Absolute::syncLength() const
{
    return value->syncLength();
}

double Absolute::getValue()
{
    const double val = fabs(value->getValue());

    if (!value->enabled)
    {
        stop();

        return 0;
    }

    return val;
}

void Absolute::init()
{
    value->start(startTime);
}

Sequence::Sequence(ValueObject* controllers, ValueObject* order) :
    controllers(controllers), order(order) {}

Sequence::~Sequence()
{
    delete controllers;
    delete order;
}

double Sequence::syncLength() const
{
    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    double length = 0;

    if (order->getLeafAs<ValueByte>()->value == Constants::Sequence::PingPong)
    {
        length += objects[0]->syncLength();

        for (size_t i = 1; i < objects.size() - 1; i++)
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

    const double value = objects[current]->getValue();

    if (!objects[current]->enabled)
    {
        last = current;

        if (++switches <= max_switches)
        {
            repeat(repeatTime + objects[current]->syncLength());

            return objects[current]->getValue();
        }

        stop();

        return 0;
    }

    return value;
}

void Sequence::init()
{
    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    switches = 0;

    chosen.clear();

    udist = std::uniform_int_distribution<size_t>(0, objects.size() - 1);

    const unsigned char orderNum = order->getLeafAs<ValueByte>()->value;

    if (orderNum == Constants::Sequence::PingPong)
    {
        max_switches = objects.size() * 2 - 3;
    }

    else
    {
        max_switches = objects.size() - 1;
    }

    if (orderNum == Constants::Sequence::Backward)
    {
        current = objects.size() - 1;
    }

    else if (orderNum == Constants::Sequence::Shuffle)
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

    switch (order->getLeafAs<ValueByte>()->value)
    {
        case Constants::Sequence::Forward:
            current = (current + 1) % objects.size();

            break;

        case Constants::Sequence::Backward:
            if (current == 0)
            {
                current = objects.size() - 1;
            }

            else
            {
                current--;
            }

            break;

        case Constants::Sequence::PingPong:
            if ((direction == -1 && current == 0) || current >= objects.size() - 1)
            {
                direction *= -1;
            }

            current += direction;

            break;

        case Constants::Sequence::Shuffle:
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

    objects[current]->start(repeatTime);
}

Repeat::Repeat(ValueObject* value, ValueObject* repeats) :
    value(value), repeats(repeats) {}

Repeat::~Repeat()
{
    delete value;
    delete repeats;
}

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
    const double val = value->getValue();

    if (!value->enabled)
    {
        const double repeatsValue = repeats->getValue();

        if (repeatsValue == 0 || ++times < repeatsValue)
        {
            repeat(repeatTime + value->syncLength());

            return value->getValue();
        }

        else
        {
            stop();

            return 0;
        }
    }

    return val;
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

Hold::Hold(ValueObject* value, ValueObject* length) :
    value(value), length(length) {}

Hold::~Hold()
{
    delete value;
    delete length;
}

double Hold::syncLength() const
{
    return length->getValue();
}

double Hold::getValue()
{
    if (utils->time - startTime >= length->getValue())
    {
        stop();

        return 0;
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

Sweep::~Sweep()
{
    delete from;
    delete to;
    delete length;
}

double Sweep::syncLength() const
{
    return length->getValue();
}

double Sweep::getValue()
{
    const double lengthValue = length->getValue();
    const double toValue = to->getValue();

    if (utils->time - startTime >= lengthValue)
    {
        stop();

        return 0;
    }

    const double fromValue = from->getValue();

    return fromValue + (toValue - fromValue) * (utils->time - startTime) / lengthValue;
}

void Sweep::init()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

LFO::LFO(ValueObject* from, ValueObject* to, ValueObject* length) :
    from(from), to(to), length(length) {}

LFO::~LFO()
{
    delete from;
    delete to;
    delete length;
}

double LFO::syncLength() const
{
    return length->getValue();
}

double LFO::getValue()
{
    const double lengthValue = length->getValue();
    const double fromValue = from->getValue();

    if (utils->time - startTime >= lengthValue)
    {
        stop();

        return 0;
    }

    return fromValue + (to->getValue() - fromValue) * (-cos(utils->twoPi * (utils->time - startTime) / lengthValue) / 2 + 0.5);
}

void LFO::init()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

Random::Random(ValueObject* from, ValueObject* to, ValueObject* length, ValueObject* type) :
    from(from), to(to), length(length), type(type) {}

Random::~Random()
{
    delete from;
    delete to;
    delete length;
    delete type;
}

double Random::syncLength() const
{
    return length->getValue();
}

double Random::getValue()
{
    const double lengthValue = length->getValue();

    if (utils->time - startTime >= lengthValue)
    {
        stop();

        return 0;
    }

    switch (type->getLeafAs<ValueByte>()->value)
    {
        case Constants::Random::Step:
            return current;

        case Constants::Random::Linear:
            return current + (next - current) * (utils->time - startTime) / lengthValue;
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

Limit::~Limit()
{
    delete value;
    delete min;
    delete max;
}

double Limit::syncLength() const
{
    return value->syncLength();
}

double Limit::getValue()
{
    if (!value->enabled)
    {
        stop();

        return 0;
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

Trigger::~Trigger()
{
    delete condition;
    delete value;
}

double Trigger::syncLength() const
{
    return value->syncLength();
}

double Trigger::getValue()
{
    if (triggered)
    {
        const double val = value->getValue();

        if (value->enabled)
        {
            return val;
        }

        stop();

        return 0;
    }

    const double cond = condition->getValue();

    if (!condition->enabled)
    {
        stop();

        return 0;
    }

    if (cond != 0)
    {
        triggered = true;

        value->start(utils->time);

        return value->getValue();
    }

    return 0;
}

void Trigger::init()
{
    triggered = false;

    condition->start(startTime);
}

If::If(ValueObject* condition, ValueObject* trueValue, ValueObject* falseValue) :
    condition(condition), trueValue(trueValue), falseValue(falseValue) {}

If::~If()
{
    delete condition;
    delete trueValue;
    delete falseValue;
}

double If::getValue()
{
    const double cond = condition->getValue();

    if (!condition->enabled)
    {
        stop();

        return 0;
    }

    if (cond == 0)
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
