#include "../include/event.h"

Event::Event(std::function<void(double)> event, std::function<void(double)> end, double startDelay, int repeats) :
    event(event), end(end), startTime(utils->time), repeats(repeats)
{
    next = startTime + startDelay;
}

bool Event::ready()
{
    return utils->time >= next;
}

void Event::perform()
{
    event(next);
}

bool Event::getNext()
{
    if (interval)
    {
        next += interval->getValue();

        if (!repeats || ++times < repeats)
        {
            return true;
        }
    }

    return false;
}

void Event::finish()
{
    end(next);
}

void EventQueue::addEvent(Event* event)
{
    events.push(event);
}

void EventQueue::performEvents()
{
    while (!events.empty() && events.top()->ready())
    {
        Event* event = events.top();

        events.pop();

        event->perform();

        if (event->getNext())
        {
            addEvent(event);
        }

        else
        {
            event->finish();
        }
    }
}

bool EventQueue::cmp::operator()(Event* left, Event* right)
{
    return left->next > right->next;
}