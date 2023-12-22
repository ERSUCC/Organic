#include "../include/controller.h"

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
    udist = std::uniform_int_distribution<>(0, controllers.size() - 1);

    if (order->order == OrderEnum::PingPong)
    {
        max_switches = controllers.size() * 2 - 1;
    }

    else
    {
        max_switches = controllers.size();
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

    switches = 0;

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

        if (++switches >= max_switches)
        {
            double value = controllers[last]->getValue();

            stop();

            return value;
        }

        controllers[current]->start(controllers[last]->getStartTime() + controllers[last]->syncLength());
    }

    return controllers[current]->getValue();
}

double Repeat::syncLength()
{
    return value->syncLength() * repeats->getValue(true);
}

double Repeat::getStartTime()
{
    return startTime - value->syncLength() * (times - 1);
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
}

double Repeat::getValueUnchecked()
{
    double val = value->getValue();

    if (utils->time - startTime >= value->syncLength())
    {
        stop();
    }

    return val;
}

Value::Value(double value) : value(value) {}

double Value::getValueUnchecked()
{
    return value;
}

double Hold::syncLength()
{
    return length->getValue(true);
}

void Hold::finishStart()
{
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
    return length->getValue(true);
}

void Sweep::finishStart()
{
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
    return length->getValue(true);
}

void LFO::finishStart()
{
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
    return length->getValue(true);
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
