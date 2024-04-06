#pragma once

#include "management.h"
#include "object.h"

struct Event : public Sync, public EventTrait
{
    Event();

    bool ready() override;
    bool getNext() override;

    double next;
};
