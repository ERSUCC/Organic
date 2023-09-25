#include "../include/event.h"

Event::Event(std::function<void(double, double)> event, std::function<void(double, double)> end, double startTime, double startDelay, double endDelay, double interval, int repeats) :
    event(event), end(end), startTime(startTime), endDelay(endDelay), interval(interval), repeats(repeats)
{
    next = startTime + startDelay;
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
            if (++times < repeats)
            {
                return true;
            }
        }

        else
        {
            return true;
        }
    }

    if (endDelay)
    {
        next += endDelay - interval.value;
        
        endDelay = 0;

        return true;
    }

    return false;
}

void Event::finish(double time)
{
    end(time, next);
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

        else
        {
            event->finish(time);
        }
    }
}

bool EventQueue::cmp::operator()(Event* left, Event* right)
{
    return left->next > right->next;
}