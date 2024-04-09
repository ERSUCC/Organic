#pragma once

#include "object.h"

struct Event : public Sync
{
    Event();

    bool ready() const;

    virtual bool hasNext() const;

    virtual void perform();

protected:
    double next;

};
