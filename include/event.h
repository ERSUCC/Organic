#pragma once

#include <functional>
#include <queue>

struct Event
{
    Event(std::function<void(double)> event, double startTime);

    bool ready(double time);
    virtual void perform(double time) = 0;
    void cancel();

    std::function<void(double)> event;

    double startTime;
    double next;

    bool discard = false;
};

struct EventQueue
{
    void addEvent(Event* event);

    void performEvents(double time);

private:
    struct cmp
    {
        bool operator()(Event* left, Event* right);
    };

    std::priority_queue<Event*, std::vector<Event*>, cmp> events;

};

struct DelayedEvent : public Event
{
    DelayedEvent(std::function<void(double)> event, double startTime, double delay);

    void perform(double time) override;
};

struct IntervalEvent : public Event
{
    IntervalEvent(std::function<void(double)> event, double startTime, double delay, double interval);

    void perform(double time) override;

    double interval;
};