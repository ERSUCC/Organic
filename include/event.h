#pragma once

#include <functional>
#include <queue>

#include "config.h"
#include "parameter.h"

struct Event
{
    friend struct EventQueue;

    Event(std::function<void(double, double)> event, std::function<void(double, double)> end, double startTime, double startDelay, double endDelay, double interval, int repeats);

    bool ready(double time);
    void perform(double time);
    bool getNext(double time);
    void finish(double time);

    Parameter interval;

private:
    std::function<void(double, double)> event;
    std::function<void(double, double)> end;

    double startTime;
    double next;
    double endDelay;

    int repeats;
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