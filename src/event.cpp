#include "../include/event.h"

Event::Event(const std::function<void(double)> action, ValueObject* delay, ValueObject* repeats, ValueObject* interval) :
    action(action), delay(delay), repeats(repeats), interval(interval) {}

void Event::update()
{
    if (utils->time >= next)
    {
        action(next);

        if (repeats->getValue() == 0 || ++times < repeats->getValue())
        {
            repeat(next);
        }

        else
        {
            stop();
        }
    }
}

void Event::init()
{
    delay->start(startTime);
    repeats->start(startTime);
    interval->start(startTime);

    next = repeatTime + delay->getValue();

    times = 0;
}

void Event::reinit()
{
    next = repeatTime + interval->getValue();
}
