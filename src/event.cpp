#include "../include/event.h"

Event::Event(const std::function<void(void)> action, ValueObject* delay) :
    action(action), delay(delay) {}

bool Event::ready() const
{
    return utils->time >= startTime + delay->getValue();
}

bool Event::hasNext() const
{
    return false;
}

void Event::perform()
{
    action();
}

void Event::finishStart()
{
    delay->start(startTime);
}

void Event::finishStop()
{
    delay->stop();
}
