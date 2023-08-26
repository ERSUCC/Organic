#include "../include/event.h"

Event::Event(std::function<void(double)> event, double startTime) : event(event), startTime(startTime) {}

void Event::cancel()
{
    discard = true;
}

DelayedEvent::DelayedEvent(std::function<void(double)> event, double startTime, double delay) : Event(event, startTime), delay(delay) {}

bool DelayedEvent::ready(double time)
{
    return time - startTime >= delay;
}

void DelayedEvent::perform(double time)
{
    event(time);

    cancel();
}

IntervalEvent::IntervalEvent(std::function<void(double)> event, double startTime, double interval) : Event(event, startTime), interval(interval), next(0) {}

bool IntervalEvent::ready(double time)
{
    return time - startTime >= next;
}

void IntervalEvent::perform(double time)
{
    event(time);

    next += interval;
}