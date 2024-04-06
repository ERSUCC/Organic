#include "../include/event.h"

Event::Event()
{
    next = startTime;
}

bool Event::ready()
{
    return utils->time >= next;
}

bool Event::getNext()
{
    return false;
}
