#pragma once

#include <functional>

#include "object.h"

struct Event : public Sync
{
    Event(const std::function<void(void)> action, ValueObject* delay);

    bool ready() const;
    bool hasNext() const;

    void perform();

protected:
    void finishStart() override;
    void finishStop() override;

    const std::function<void(void)> action;

    ValueObject* delay;

    double next;

};
