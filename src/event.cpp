#include "../include/event.h"

Event::Event()
{
    next = startTime;
}

bool Event::ready() const
{
    return utils->time >= next;
}

bool Event::hasNext() const
{
    return false;
}

void Event::perform() {}
