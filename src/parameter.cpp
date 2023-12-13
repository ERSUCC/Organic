#include "../include/parameter.h"

void ParameterController::finishStop()
{
    if (repeats->getValue() == 0 || ++times < repeats->getValue())
    {
        start(startTime + syncLength());
    }
}

ControllerGroup::Order::Order(OrderEnum order) : order(order) {}

double ControllerGroup::syncLength()
{
    double length = 0;

    for (ValueObject* controller : controllers)
    {
        length += controller->syncLength();
    }

    return length;
}

void ControllerGroup::finishStart()
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

    else
    {
        current = 0;
    }

    times = 0;

    controllers[current]->start(startTime);
}

double ControllerGroup::getValueUnchecked()
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
            stop();

            return controllers[last]->getValue();
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
    if (utils->time - startTime >= length->getValue())
    {
        stop();
    }

    return value->getValue();
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
    if (utils->time - startTime >= length->getValue())
    {
        stop();
    }

    return from->getValue() + (to->getValue() - from->getValue()) * (utils->time - startTime) / length->getValue();
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
    if (utils->time >= startTime + length->getValue())
    {
        stop();
    }

    return from->getValue() + (to->getValue() - from->getValue()) * (-cos(utils->twoPi * (utils->time - startTime) / length->getValue()) / 2 + 0.5);
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

    current = next;
    next = udist(utils->rng);
}

double Random::getValueUnchecked()
{
    if (utils->time >= startTime + length->getValue())
    {
        stop();
    }

    switch (type->type)
    {
        case TypeEnum::Step:
            return current;

        case TypeEnum::Linear:
            return current + (next - current) * (utils->time - startTime) / length->getValue();
    }
}