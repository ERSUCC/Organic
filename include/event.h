#pragma once

#include <functional>

struct Event
{
    Event(std::function<void(double)> event, double startTime);

    virtual bool ready(double time) = 0;
    virtual void perform(double time) = 0;
    void cancel();

    std::function<void(double)> event;

    double startTime;

    bool discard = false;
};

struct DelayedEvent : public Event
{
    DelayedEvent(std::function<void(double)> event, double startTime, double delay);

    bool ready(double time) override;
    void perform(double time) override;

    double delay;
};

struct IntervalEvent : public Event
{
    IntervalEvent(std::function<void(double)> event, double startTime, double interval);

    bool ready(double time) override;
    void perform(double time) override;

    double interval;
    double next;
};