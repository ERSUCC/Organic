#include "../include/event.h"

Event::Event(std::function<void(double, double)> event, double startTime) : event(event), startTime(startTime) {}

bool Event::ready(double time)
{
    return time >= next;
}

void Event::cancel()
{
    discard = true;
}

void EventQueue::addEvent(Event* event)
{
    events.push(event);
}

void EventQueue::performEvents(double time)
{
    while (!events.empty() && events.top()->ready(time))
    {
        Event* event = events.top();

        events.pop();

        event->perform(time);

        if (!event->discard)
        {
            addEvent(event);
        }
    }
}

bool EventQueue::cmp::operator()(Event* left, Event* right)
{
    return left->next > right->next;
}

DelayedEvent::DelayedEvent(std::function<void(double, double)> event, double startTime, double delay) : Event(event, startTime)
{
    next = startTime + delay;
}

void DelayedEvent::perform(double time)
{
    event(time, next);

    cancel();
}

RepeatedEvent::RepeatedEvent(std::function<void(double, double)> event, double startTime, double delay, double interval, int repeats) : Event(event, startTime), interval(interval), repeats(repeats)
{
    next = startTime + delay;
}

void RepeatedEvent::perform(double time)
{
    if (++times <= repeats)
    {
        event(time, next);

        next += interval;
    }

    else
    {
        cancel();
    }
}

IntervalEvent::IntervalEvent(std::function<void(double, double)> event, double startTime, double delay, double interval) : Event(event, startTime), interval(interval)
{
    next = startTime + delay;
}

void IntervalEvent::perform(double time)
{
    event(time, next);

    next += interval;
}