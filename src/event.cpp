#include "../include/event.h"

Event::Event(std::function<void(double, double)> event, double startTime, double delay, double interval, int repeats) :
    event(event), startTime(startTime), interval(interval), repeats(repeats)
{
    next = startTime + delay;
}

bool Event::ready(double time)
{
    return time >= next;
}

void Event::perform(double time)
{
    event(time, next);
}

bool Event::getNext(double time)
{
    if (interval.value)
    {
        next += interval.value;

        if (repeats)
        {
            return ++times < repeats;
        }

        return true;
    }

    return false;
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

        if (event->getNext(time))
        {
            addEvent(event);
        }
    }
}

bool EventQueue::cmp::operator()(Event* left, Event* right)
{
    return left->next > right->next;
}

RhythmEvent::RhythmEvent(std::function<void(double, double)> event, double startTime, double delay, double interval, int repeats, std::vector<double> rhythm) :
    Event(event, startTime, delay, interval, repeats), rhythm(rhythm) {}

bool RhythmEvent::getNext(double time)
{
    if (current < rhythm.size())
    {
        next += rhythm[current++];

        return true;
    }

    if (interval.value)
    {
        next += interval.value;

        current = 0;

        if (repeats)
        {
            return ++times < repeats;
        }

        return true;
    }

    return false;
}