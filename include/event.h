#pragma once

#include <functional>
#include <queue>
#include <random>

#include "config.h"

struct Event
{
    Event(std::function<void(double, double)> event, double startTime);

    bool ready(double time);
    virtual void perform(double time) = 0;
    void cancel();

    std::function<void(double, double)> event;

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
    DelayedEvent(std::function<void(double, double)> event, double startTime, double delay);

    void perform(double time) override;
};

struct RepeatedEvent : public Event
{
    RepeatedEvent(std::function<void(double, double)> event, double startTime, double delay, double interval, int repeats);

    void perform(double time) override;

    double interval;

    int repeats;
    int times = 0;
};

struct RandomRepeatedEvent : public Event
{
    RandomRepeatedEvent(std::function<void(double, double)> event, double startTime, double delay, double floor, double ceiling, double step, int repeats);

    void perform(double time) override;

    double floor;
    double ceiling;
    double step;

    int repeats;
    int times = 0;

private:
    std::uniform_real_distribution<> udist;

};

struct IntervalEvent : public Event
{
    IntervalEvent(std::function<void(double, double)> event, double startTime, double delay, double interval);

    void perform(double time) override;

    double interval;
};

struct RandomIntervalEvent : public Event
{
    RandomIntervalEvent(std::function<void(double, double)> event, double startTime, double delay, double floor, double ceiling, double step);

    void perform(double time) override;

    double floor;
    double ceiling;
    double step;

private:
    std::uniform_real_distribution<> udist;

};