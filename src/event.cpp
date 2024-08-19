#include "../include/event.h"

Event::Event(const std::function<void(double)> action, ValueObject* delay, ValueObject* repeats, ValueObject* interval) :
    action(action), delay(delay), repeats(repeats), interval(interval) {}

bool Event::ready() const
{
    return utils->time >= next;
}

bool Event::hasNext() const
{
    return repeats->getValue() == 0 || times < repeats->getValue();
}

void Event::perform()
{
    action(next);

    times++;

    next += interval->getValue();
}

void Event::finishStart()
{
    delay->start(startTime);
    repeats->start(startTime);
    interval->start(startTime);

    times = 0;

    next = startTime + delay->getValue();
}
