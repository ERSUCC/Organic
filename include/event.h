#pragma once

#include <functional>

#include "object.h"

struct Event : public Sync
{
    Event(const std::function<void(void)> action);

    bool ready() const;
    bool hasNext() const;

    void perform();

protected:
    const std::function<void(void)> action;

    double next;

};
