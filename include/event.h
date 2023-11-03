#pragma once

#include <functional>
#include <queue>

#include "config.h"
#include "parameter.h"

struct Event : public Sync
{
    friend struct EventQueue;

    Event(std::function<void(double)> event, std::function<void(double)> end, double startDelay, double interval, int repeats);

    bool ready();
    void perform();
    bool getNext();
    void finish();

    Parameter interval;

private:
    std::function<void(double)> event;
    std::function<void(double)> end;

    double startTime;
    double next;

    int repeats;
    int times = 0;

};

struct EventQueue
{
    void addEvent(Event* event);

    void performEvents();

private:
    struct cmp
    {
        bool operator()(Event* left, Event* right);
    };

    std::priority_queue<Event*, std::vector<Event*>, cmp> events;

};