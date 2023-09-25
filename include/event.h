#pragma once

#include <functional>
#include <queue>

#include "config.h"
#include "parameter.h"

struct Event
{
    Event(std::function<void(double, double)> event, double startTime, double delay, double interval, int repeats);

    bool ready(double time);
    virtual void perform(double time);
    virtual bool getNext(double time);

    std::function<void(double, double)> event;

    double startTime;
    double next;

    int repeats;

    Parameter interval;

protected:
    int times = 0;

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

struct RhythmEvent : public Event
{
    RhythmEvent(std::function<void(double, double)> event, double startTime, double delay, double interval, int repeats, std::vector<double> rhythm);

    bool getNext(double time) override;

private:
    std::vector<double> rhythm;

    int current = 0;

};