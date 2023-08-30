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