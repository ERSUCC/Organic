#include "../include/event.h"

Event::Event(std::function<void(double)> event, double startTime) : event(event), startTime(startTime) {}

bool Event::ready(double time)
{
    return time - startTime >= next;
}

void Event::cancel()
{
    discard = true;
}

DelayedEvent::DelayedEvent(std::function<void(double)> event, double startTime, double delay) : Event(event, startTime)
{
    next = delay;
}

void DelayedEvent::perform(double time)
{
    event(time);

    cancel();
}

IntervalEvent::IntervalEvent(std::function<void(double)> event, double startTime, double delay, double interval) : Event(event, startTime), interval(interval)
{
    next = delay;
}

void IntervalEvent::perform(double time)
{
    event(time);

    next += interval;
}