#pragma once

#include <functional>

#include "object.h"

struct Event : public Sync
{
    Event(const std::function<void(double)> action, ValueObject* delay, ValueObject* repeats, ValueObject* interval);

    void update();

protected:
    void init() override;
    void reinit() override;

    const std::function<void(double)> action;

    ValueObject* delay;
    ValueObject* repeats;
    ValueObject* interval;

    double delayTime;

    unsigned int times;

};
