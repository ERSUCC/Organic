#include "../include/parameter.h"

void ParameterController::finishStop()
{
    if (repeats->getValue() == 0 || ++times < repeats->getValue())
    {
        start();
    }
}

ControllerGroup::Order::Order(OrderEnum order) : order(order) {}

void ControllerGroup::finishStart()
{
    repeats->start();

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

    controllers[current]->start();
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

            return controllers[controllers.size() - 1]->getValue();
        }

        controllers[current]->start();
    }

    return controllers[current]->getValue();
}

Value::Value(double value) : value(value) {}

double Value::getValueUnchecked()
{
    return value;
}

void Hold::finishStart()
{
    repeats->start();
    value->start();
    length->start();
}

double Hold::getValueUnchecked()
{
    if (utils->time - startTime >= length->getValue())
    {
        stop();
    }

    return value->getValue();
}

void Sweep::finishStart()
{
    repeats->start();
    from->start();
    to->start();
    length->start();
}

double Sweep::getValueUnchecked()
{
    if (utils->time - startTime >= length->getValue())
    {
        stop();

        return to->getValue();
    }

    return from->getValue() + (to->getValue() - from->getValue()) * (utils->time - startTime) / length->getValue();
}

void LFO::finishStart()
{
    repeats->start();
    from->start();
    to->start();
    length->start();
}

double LFO::getValueUnchecked()
{
    if (utils->time >= startTime + length->getValue())
    {
        stop();
    }

    return from->getValue() + (to->getValue() - from->getValue()) * (-cos(utils->twoPi * (utils->time - startTime) / length->getValue()) / 2 + 0.5);
}