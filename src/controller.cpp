#include "../include/controller.h"

void ValueController::finishStop()
{
    if (repeats->getValue() == 0 || ++times < repeats->getValue())
    {
        start(startTime + syncLength());
    }
}

Sequence::Order::Order(OrderEnum order) : order(order) {}

double Sequence::syncLength()
{
    double length = 0;
    
    for (ValueObject* controller : controllers)
    {
        length += controller->syncLength();
    }

    return length;
}

void Sequence::finishStart()
{
    repeats->start(startTime);

    udist = std::uniform_int_distribution<>(0, controllers.size() - 1);

    if (order->order == OrderEnum::PingPong)
    {
        max_times = controllers.size() * 2 - 1;
    }

    else
    {
        max_times = controllers.size();
    }

    if (order->order == OrderEnum::Backwards)
    {
        current = controllers.size() - 1;
    }

    else if (order->order == OrderEnum::Random)
    {
        current = udist(utils->rng);
    }

    else
    {
        current = 0;
    }

    times = 0;

    controllers[current]->start(startTime);
}

double Sequence::getValueUnchecked()
{
    if (!controllers[current]->enabled)
    {
        last = current;

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
                current = udist(utils->rng);

                if (current == last)
                {
                    current = (current + 1) % controllers.size();
                }

                break;
            }
        }

        if (++times >= max_times)
        {
            double value = controllers[last]->getValue();

            stop();

            return value;
        }

        controllers[current]->start(controllers[last]->startTime + controllers[last]->syncLength());
    }

    return controllers[current]->getValue();
}

Value::Value(double value) : value(value) {}

double Value::getValueUnchecked()
{
    return value;
}

Hold::Hold()
{
    repeats = new Value(1);
}

double Hold::syncLength()
{
    return length->getValue();
}

void Hold::finishStart()
{
    repeats->start(startTime);
    value->start(startTime);
    length->start(startTime);
}

double Hold::getValueUnchecked()
{
    double val = value->getValue();

    if (utils->time - startTime >= length->getValue())
    {
        stop();
    }

    return val;
}

double Sweep::syncLength()
{
    return length->getValue();
}

void Sweep::finishStart()
{
    repeats->start(startTime);
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

double Sweep::getValueUnchecked()
{
    double value = from->getValue() + (to->getValue() - from->getValue()) * (utils->time - startTime) / length->getValue();

    if (utils->time - startTime >= length->getValue())
    {
        stop();
    }

    return value;
}

double LFO::syncLength()
{
    return length->getValue();
}

void LFO::finishStart()
{
    repeats->start(startTime);
    from->start(startTime);
    to->start(startTime);
    length->start(startTime);
}

double LFO::getValueUnchecked()
{
    double value = from->getValue() + (to->getValue() - from->getValue()) * (-cos(utils->twoPi * (utils->time - startTime) / length->getValue()) / 2 + 0.5);

    if (utils->time >= startTime + length->getValue())
    {
        stop();
    }

    return value;
}

Random::Type::Type(TypeEnum type) : type(type) {}

double Random::syncLength()
{
    return length->getValue();
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

double Random::getValueUnchecked()
{
    double value = 0;

    switch (type->type)
    {
        case TypeEnum::Step:
            return current;

        case TypeEnum::Linear:
            return current + (next - current) * (utils->time - startTime) / length->getValue();
    }
    
    if (utils->time >= startTime + length->getValue())
    {
        stop();
    }

    return value;
}
