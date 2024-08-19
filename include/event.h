#pragma once

#include <functional>

#include "object.h"

struct Event : public Sync
{
    Event(const std::function<void(double)> action, ValueObject* delay, ValueObject* repeats, ValueObject* interval);

    bool ready() const;
    bool hasNext() const;

    void perform();

protected:
    void init() override;

    const std::function<void(double)> action;

    ValueObject* delay;
    ValueObject* repeats;
    ValueObject* interval;

    double next;

    unsigned int times;

};
