#include "../include/controller.h"

using namespace Engine;

double Time::getValue() const
{
    return utils->time;
}

Value::Value(const double value) :
    value(value) {}

double Value::getValue() const
{
    return value;
}

ValueChar::ValueChar(const unsigned char value) :
    value(value) {}

ValueNegate::ValueNegate(ValueObject* value) :
    value(value) {}

ValueNegate::~ValueNegate()
{
    delete value;
}

double ValueNegate::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return -value->getValue();
}

void ValueNegate::update()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
}

void ValueNegate::init()
{
    value->start(startTime);
}

ValueCombination::ValueCombination(ValueObject* value1, ValueObject* value2) :
    value1(value1), value2(value2) {}

ValueCombination::~ValueCombination()
{
    delete value1;
    delete value2;
}

double ValueCombination::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return getValueInternal(value1->getValue(), value2->getValue());
}

void ValueCombination::update()
{
    value1->update();
    value2->update();

    if (!value1->enabled)
    {
        stop(value1->getStopTime());
    }

    else if (!value2->enabled)
    {
        stop(value2->getStopTime());
    }
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

double All::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    for (const ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (object->getValue() == 0)
        {
            return 0;
        }
    }

    return 1;
}

void All::update()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->update();

        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

void All::init()
{
    values->start(startTime);

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

Any::Any(ValueObject* values) :
    values(values) {}

Any::~Any()
{
    delete values;
}

double Any::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    for (const ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (object->getValue() != 0)
        {
            return 1;
        }
    }

    return 0;
}

void Any::update()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->update();

        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

void Any::init()
{
    values->start(startTime);

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

None::None(ValueObject* values) :
    values(values) {}

None::~None()
{
    delete values;
}

double None::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    for (const ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (object->getValue() != 0)
        {
            return 0;
        }
    }

    return 1;
}

void None::update()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->update();

        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

void None::init()
{
    values->start(startTime);

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

Min::Min(ValueObject* values) :
    values(values) {}

Min::~Min()
{
    delete values;
}

double Min::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    double min = utils->infinity;

    for (const ValueObject* object : values->getLeafAs<List>()->objects)
    {
        const double value = object->getValue();

        if (value < min)
        {
            min = value;
        }
    }

    return min;
}

void Min::update()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->update();

        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

void Min::init()
{
    values->start(startTime);

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

Max::Max(ValueObject* values) :
    values(values) {}

Max::~Max()
{
    delete values;
}

double Max::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    double max = -utils->infinity;

    for (const ValueObject* object : values->getLeafAs<List>()->objects)
    {
        const double value = object->getValue();

        if (value > max)
        {
            max = value;
        }
    }

    return max;
}

void Max::update()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->update();

        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

void Max::init()
{
    values->start(startTime);

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        object->start(startTime);
    }
}

Round::Round(ValueObject* value, ValueObject* step, ValueObject* direction) :
    value(value), step(step), direction(direction) {}

Round::~Round()
{
    delete value;
    delete step;
    delete direction;
}

double Round::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    const double val = value->getValue();
    const double st = step->getValue();

    if (st == 0)
    {
        return val;
    }

    switch (direction->getLeafAs<ValueChar>()->value)
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

void Round::update()
{
    value->update();
    step->update();
    direction->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
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

double Absolute::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return fabs(value->getValue());
}

void Absolute::update()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
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

double Sequence::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return controllers->getLeafAs<List>()->objects[current]->getValue();
}

ValueObject* Sequence::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return controllers->getLeafAs<List>()->objects[current]->getLeaf();
}

void Sequence::update()
{
    controllers->update();
    order->update();

    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    ValueObject* object = objects[current];

    if (!order->enabled)
    {
        const double stopTime = order->getStopTime();

        object->stop(stopTime);

        stop(stopTime);

        return;
    }

    object->update();

    if (!object->enabled)
    {
        last = current;

        if (++switches < objects.size())
        {
            repeat(object->getStopTime());
        }

        else
        {
            stop(object->getStopTime());
        }
    }
}

void Sequence::init()
{
    controllers->start(startTime);
    order->start(startTime);

    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    switches = 0;

    chosen.clear();

    udist = std::uniform_int_distribution<size_t>(0, objects.size() - 1);

    switch (order->getLeafAs<ValueChar>()->value)
    {
        case Constants::Sequence::Backward:
            current = objects.size() - 1;

            break;

        case Constants::Sequence::Shuffle:
            current = udist(utils->rng);

            if (current == last)
            {
                current = (current + 1) % objects.size();
            }

            chosen.insert(current);

            break;

        default:
            current = 0;

            break;
    }

    objects[current]->start(startTime);
}

void Sequence::reinit()
{
    const std::vector<ValueObject*>& objects = controllers->getLeafAs<List>()->objects;

    switch (order->getLeafAs<ValueChar>()->value)
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

double Repeat::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return value->getValue();
}

ValueObject* Repeat::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return value->getLeaf();
}

void Repeat::update()
{
    value->update();
    repeats->update();

    if (!value->enabled)
    {
        const double repeatsValue = repeats->getValue();

        if (repeatsValue == 0 || ++times < repeatsValue)
        {
            repeat(value->getStopTime());
        }

        else
        {
            stop(value->getStopTime());
        }
    }
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
    repeats->start(startTime);
}

Hold::Hold(ValueObject* value, ValueObject* length) :
    value(value), length(length) {}

Hold::~Hold()
{
    delete value;
    delete length;
}

double Hold::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return value->getValue();
}

ValueObject* Hold::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return value;
}

void Hold::update()
{
    value->update();
    length->update();

    const double lengthValue = length->getValue();

    if (utils->time - startTime >= lengthValue)
    {
        stop(startTime + lengthValue);
    }
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

double Sweep::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    const double fromValue = from->getValue();
    const double toValue = to->getValue();
    const double lengthValue = length->getValue();

    return fromValue + (toValue - fromValue) * (utils->time - startTime) / lengthValue;
}

void Sweep::update()
{
    from->update();
    to->update();
    length->update();

    const double lengthValue = length->getValue();

    if (utils->time - startTime >= lengthValue)
    {
        stop(startTime + lengthValue);
    }
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

double LFO::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    const double fromValue = from->getValue();
    const double toValue = to->getValue();
    const double lengthValue = length->getValue();

    return fromValue + (toValue - fromValue) * (-cos(utils->twoPi * (utils->time - startTime) / lengthValue) / 2 + 0.5);
}

void LFO::update()
{
    from->update();
    to->update();
    length->update();

    const double lengthValue = length->getValue();

    if (utils->time - startTime >= lengthValue)
    {
        stop(startTime + lengthValue);
    }
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

double Random::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    switch (type->getLeafAs<ValueChar>()->value)
    {
        case Constants::Random::Step:
            return current;

        case Constants::Random::Linear:
            return current + (next - current) * (utils->time - startTime) / length->getValue();
    }

    return 0;
}

void Random::update()
{
    from->update();
    to->update();
    length->update();
    type->update();

    const double lengthValue = length->getValue();

    if (utils->time - startTime >= lengthValue)
    {
        stop(startTime + lengthValue);
    }
}

void Random::init()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
    type->start(startTime);

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

double Limit::getValue() const
{
    if (!enabled)
    {
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

void Limit::update()
{
    value->update();
    min->update();
    max->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
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

double Trigger::getValue() const
{
    if (!enabled || !value->enabled)
    {
        return 0;
    }

    return value->getValue();
}

ValueObject* Trigger::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return value->getLeaf();
}

void Trigger::update()
{
    if (triggered)
    {
        condition->update();
        value->update();

        if (!value->enabled)
        {
            stop(value->getStopTime());
        }
    }

    else
    {
        condition->update();

        if (!condition->enabled)
        {
            stop(condition->getStopTime());
        }

        else if (condition->getValue() != 0)
        {
            triggered = true;

            value->start(utils->time);
        }
    }
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

double If::getValue() const
{
    if (condition->getValue() == 0)
    {
        return falseValue->getValue();
    }

    return trueValue->getValue();
}

ValueObject* If::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    if (condition->getValue() == 0)
    {
        return falseValue->getLeaf();
    }

    return trueValue->getLeaf();
}

void If::update()
{
    condition->update();
    trueValue->update();
    falseValue->update();

    if (!condition->enabled)
    {
        stop(condition->getStopTime());
    }
}

void If::init()
{
    condition->start(startTime);
    trueValue->start(startTime);
    falseValue->start(startTime);
}
