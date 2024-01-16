#include "../include/controller.h"

double Variable::syncLength()
{
    return value->syncLength();
}

double Variable::getValue()
{
    if (!value->enabled)
    {
        stop();
    }

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

Sequence::Order::Order(OrderEnum order) :
    order(order) {}

Sequence::Sequence(std::vector<ValueObject*> controllers, Order* order) :
    controllers(controllers), order(order) {}

double Sequence::syncLength()
{
    double length = 0;
    
    for (ValueObject* controller : controllers)
    {
        length += controller->syncLength();
    }

    return length;
}

double Sequence::getValue()
{
    if (!controllers[current]->enabled)
    {
        last = current;

        if (++switches <= max_switches)
        {
            repeat(repeatTime + controllers[current]->syncLength());
        }

        else
        {
            stop();
        }
    }

    return controllers[current]->getValue();
}

void Sequence::finishStart()
{
    switches = 0;

    chosen.clear();

    udist = std::uniform_int_distribution<>(0, controllers.size() - 1);

    if (order->order == OrderEnum::PingPong)
    {
        max_switches = controllers.size() * 2 - 2;
    }

    else
    {
        max_switches = controllers.size() - 1;
    }

    if (order->order == OrderEnum::Backwards)
    {
        current = controllers.size() - 1;
    }

    else if (order->order == OrderEnum::Random)
    {
        current = udist(utils->rng);

        if (current == last)
        {
            current = (current + 1) % controllers.size();
        }

        chosen.insert(current);
    }

    else
    {
        current = 0;
    }

    controllers[current]->start(startTime);
}

void Sequence::finishRepeat()
{
    switch (order->order)
    {
        case OrderEnum::Forwards:
            current = (current + 1) % controllers.size();

            break;

        case OrderEnum::Backwards:
        {
            current -= 1;

            if (current < 0)
            {
                current = controllers.size() - 1;
            }

            break;
        }

        case OrderEnum::PingPong:
        {
            if ((direction == -1 && current <= 0) || current >= controllers.size() - 1)
            {
                direction *= -1;
            }

            current += direction;

            break;
        }

        case OrderEnum::Random:
        {
            if (chosen.size() < controllers.size())
            {
                current = udist(utils->rng);

                while (chosen.count(current))
                {
                    current = (current + 1) % controllers.size();
                }

                chosen.insert(current);
            }

            break;
        }
    }

    controllers[current]->start(repeatTime);
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

void Repeat::finishStart()
{
    value->start(startTime);
    repeats->start(startTime);

    times = 0;
}

void Repeat::finishRepeat()
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

void Hold::finishStart()
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

void Sweep::finishStart()
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

        return to->getValue();
    }

    return from->getValue() + (to->getValue() - from->getValue()) * (-cos(utils->twoPi * (utils->time - startTime) / syncLength()) / 2 + 0.5);
}

void LFO::finishStart()
{
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

Random::Type::Type(TypeEnum type) :
    type(type) {}

Random::Random(ValueObject* from, ValueObject* to, ValueObject* length, Type* type) :
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

    switch (type->type)
    {
        case TypeEnum::Step:
            return current;

        case TypeEnum::Linear:
            return current + (next - current) * (utils->time - startTime) / syncLength();
    }
}

void Random::finishStart()
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
