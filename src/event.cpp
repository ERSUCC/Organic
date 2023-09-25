#include "../include/event.h"

Event::Event(std::function<void(double, double)> event, double startTime, double delay) : event(event), startTime(startTime)
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

    cancel();
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

RepeatedEvent::RepeatedEvent(std::function<void(double, double)> event, double startTime, double delay, double interval, int repeats) :
    Event(event, startTime, delay), interval(interval), repeats(repeats) {}

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

RandomRepeatedEvent::RandomRepeatedEvent(std::function<void(double, double)> event, double startTime, double delay, double floor, double ceiling, double step, int repeats) :
    Event(event, startTime, delay), floor(floor), ceiling(ceiling), step(step), repeats(repeats)
{
    udist = std::uniform_real_distribution<>(0, ceiling - floor);
}

void RandomRepeatedEvent::perform(double time)
{
    if (++times <= repeats)
    {
        event(time, next);

        next += step * round(udist(Config::RNG) / step) + floor;
    }

    else
    {
        cancel();
    }
}

IntervalEvent::IntervalEvent(std::function<void(double, double)> event, double startTime, double delay, double interval) :
    Event(event, startTime, delay), interval(interval) {}

void IntervalEvent::perform(double time)
{
    event(time, next);

    next += interval;
}

RandomIntervalEvent::RandomIntervalEvent(std::function<void(double, double)> event, double startTime, double delay, double floor, double ceiling, double step) :
    Event(event, startTime, delay), floor(floor), ceiling(ceiling), step(step)
{
    udist = std::uniform_real_distribution<>(0, ceiling - floor);
}

void RandomIntervalEvent::perform(double time)
{
    event(time, next);

    next += step * round(udist(Config::RNG) / step) + floor;
}

RhythmEvent::RhythmEvent(std::function<void(double, double)> event, double startTime, double delay, std::vector<double> rhythm) :
    Event(event, startTime, delay), rhythm(rhythm) {}

void RhythmEvent::perform(double time)
{
    event(time, next);

    next += rhythm[current];

    current = (current + 1) % rhythm.size();
}