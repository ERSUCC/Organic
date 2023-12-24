#include "../include/controller.h"

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
    if (utils->time - startTime >= controllers[current]->syncLength())
    {
        stop();
    }

    return controllers[current]->getValue();
}

void Sequence::finishStart()
{
    if (switches == 0)
    {
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
    }

    controllers[current]->stop();
    controllers[current]->start(startTime);
}

void Sequence::finishStop()
{
    last = current;

    if (++switches <= max_switches)
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

        start(startTime + controllers[last]->syncLength());
    }

    else
    {
        switches = 0;

        chosen.clear();
    }
}

Repeat::Repeat(ValueObject* value, ValueObject* repeats) :
    value(value), repeats(repeats) {}

double Repeat::syncLength()
{
    return value->syncLength() * repeats->getValue();
}

double Repeat::getStartTime()
{
    if (times == 0)
    {
        return startTime - syncLength();
    }

    return startTime - value->syncLength() * (times - 1);
}

double Repeat::getValue()
{
    if (utils->time - startTime >= value->syncLength())
    {
        stop();
    }

    return value->getValue();
}

void Repeat::finishStart()
{
    value->stop();
    value->start(startTime);
    repeats->start(startTime);
}

void Repeat::finishStop()
{
    if (repeats->getValue() == 0 || ++times < repeats->getValue())
    {
        start(startTime + value->syncLength());
    }

    else
    {
        times = 0;
    }
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
