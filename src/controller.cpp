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

void ValueNegate::init()
{
    value->start(startTime);
}

void ValueNegate::compute()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
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

void ValueCombination::init()
{
    value1->start(startTime);
    value2->start(startTime);
}

void ValueCombination::compute()
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
    values(new MultiList(values)) {}

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

void All::init()
{
    values->start(startTime);
}

void All::compute()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

Any::Any(ValueObject* values) :
    values(new MultiList(values)) {}

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

void Any::init()
{
    values->start(startTime);
}

void Any::compute()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

None::None(ValueObject* values) :
    values(new MultiList(values)) {}

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

void None::init()
{
    values->start(startTime);
}

void None::compute()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

Min::Min(ValueObject* values) :
    values(new MultiList(values)) {}

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

void Min::init()
{
    values->start(startTime);
}

void Min::compute()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
    }
}

Max::Max(ValueObject* values) :
    values(new MultiList(values)) {}

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

void Max::init()
{
    values->start(startTime);
}

void Max::compute()
{
    values->update();

    if (!values->enabled)
    {
        stop(values->getStopTime());

        return;
    }

    for (ValueObject* object : values->getLeafAs<List>()->objects)
    {
        if (!object->enabled)
        {
            stop(object->getStopTime());

            return;
        }
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

void Round::init()
{
    value->start(startTime);
    step->start(startTime);
    direction->start(startTime);
}

void Round::compute()
{
    value->update();
    step->update();
    direction->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
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

void Absolute::init()
{
    value->start(startTime);
}

void Absolute::compute()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
}

Modulo::Modulo(ValueObject* value, ValueObject* divisor) :
    value(value), divisor(divisor) {}

Modulo::~Modulo()
{
    delete value;
    delete divisor;
}

double Modulo::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    const double divisorValue = divisor->getValue();

    if (divisorValue == 0)
    {
        return 0;
    }

    return fmod(value->getValue(), divisorValue);
}

void Modulo::init()
{
    value->start(startTime);
    divisor->start(startTime);
}

void Modulo::compute()
{
    value->update();
    divisor->update();

    if (!value->enabled || !divisor->enabled)
    {
        stop(value->getStopTime());
    }
}

Logarithm::Logarithm(ValueObject* value, ValueObject* base) :
    value(value), base(base) {}

Logarithm::~Logarithm()
{
    delete value;
    delete base;
}

double Logarithm::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    const double valueValue = value->getValue();
    const double baseValue = base->getValue();

    if (valueValue <= 0 || baseValue <= 0 || baseValue == 1)
    {
        return 0;
    }

    return log(valueValue) / log(baseValue);
}

void Logarithm::init()
{
    value->start(startTime);
    base->start(startTime);
}

void Logarithm::compute()
{
    value->update();
    base->update();

    if (!value->enabled || !base->enabled)
    {
        stop(value->getStopTime());
    }
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

void Sequence::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    controllers->getLeaves(leaves);
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

void Sequence::compute()
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

void Repeat::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    value->getLeaves(leaves);
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

void Repeat::compute()
{
    value->update();
    repeats->update();

    if (!value->enabled)
    {
        const double repeatsValue = repeats->getValue();

        if (repeats->enabled && (repeatsValue == 0 || ++times < repeatsValue))
        {
            repeat(value->getStopTime());
        }

        else
        {
            stop(value->getStopTime());
        }
    }
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

void Hold::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    value->getLeaves(leaves);
}

void Hold::init()
{
    value->start(startTime);
    length->start(startTime);
}

void Hold::compute()
{
    value->update();
    length->update();

    const double lengthValue = length->getValue();

    if (utils->time - startTime >= lengthValue)
    {
        stop(startTime + lengthValue);
    }
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

void Sweep::init()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

void Sweep::compute()
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

void LFO::init()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

void LFO::compute()
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

Envelope::Envelope(ValueObject* from, ValueObject* to, ValueObject* attack, ValueObject* sustain, ValueObject* release) :
    from(from), to(to), attack(attack), sustain(sustain), release(release) {}

Envelope::~Envelope()
{
    delete from;
    delete to;
    delete attack;
    delete sustain;
    delete release;
}

double Envelope::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    const double fromValue = from->getValue();
    const double toValue = to->getValue();
    const double attackValue = attack->getValue();
    const double sustainValue = sustain->getValue();
    const double releaseValue = release->getValue();

    const double time = utils->time - startTime;

    if (time < attackValue)
    {
        return fromValue + (toValue - fromValue) * time / attackValue;
    }

    if (time < attackValue + sustainValue)
    {
        return toValue;
    }

    return toValue - (toValue - fromValue) * (time - attackValue - sustainValue) / releaseValue;
}

void Envelope::init()
{
    from->start(startTime);
    to->start(startTime);
    attack->start(startTime);
    sustain->start(startTime);
    release->start(startTime);
}

void Envelope::compute()
{
    from->update();
    to->update();
    attack->update();
    sustain->update();
    release->update();

    const double attackValue = attack->getValue();
    const double sustainValue = sustain->getValue();
    const double releaseValue = release->getValue();

    const double length = attackValue + sustainValue + releaseValue;

    if (utils->time - startTime >= length)
    {
        stop(startTime + length);
    }
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
        case Constants::Random::Stay:
            return current;

        case Constants::Random::Linear:
            return current + (next - current) * (utils->time - startTime) / length->getValue();
    }

    return 0;
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

void Random::compute()
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

void Limit::init()
{
    value->start(startTime);
    min->start(startTime);
    max->start(startTime);
}

void Limit::compute()
{
    value->update();
    min->update();
    max->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
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

void Trigger::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    value->getLeaves(leaves);
}

void Trigger::init()
{
    condition->start(startTime);
}

void Trigger::compute()
{
    if (triggered)
    {
        condition->update();
        value->update();

        if (!value->enabled)
        {
            triggered = condition->getValue() != 0;

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

void If::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    trueValue->getLeaves(leaves);
    falseValue->getLeaves(leaves);
}

void If::init()
{
    condition->start(startTime);
    trueValue->start(startTime);
    falseValue->start(startTime);
}

void If::compute()
{
    condition->update();
    trueValue->update();
    falseValue->update();

    if (!condition->enabled)
    {
        stop(condition->getStopTime());
    }
}

Amplitude::Amplitude(ValueObject* source) :
    source(source)
{
    buffer = (double*)malloc(sizeof(double) * utils->channels);
}

Amplitude::~Amplitude()
{
    delete source;

    free(buffer);
}

double Amplitude::getValue() const
{
    double average = 0;

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        average += buffer[i];
    }

    return average / utils->channels;
}

void Amplitude::init()
{
    source->start(startTime);
}

void Amplitude::compute()
{
    source->update();

    memset(buffer, 0, sizeof(double) * utils->channels);

    source->getLeafAs<AudioSource>()->fillBuffer(buffer);
}
