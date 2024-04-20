#include "../include/event.h"

Event::Event(const std::function<void(void)> action) :
    action(action)
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

void Event::perform()
{
    action();
}
