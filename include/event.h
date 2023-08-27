#pragma once

#include <functional>

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