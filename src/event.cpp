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

AssignEvent::AssignEvent(Variable* variable, ValueObject* value) :
    variable(variable), value(value) {}

void AssignEvent::perform()
{
    variable->value = value;

    value->start(next);
}

CopyEvent::CopyEvent(Variable* from, Variable* to) :
    from(from), to(to) {}

void CopyEvent::perform()
{
    to->value = from->value;

    to->value->start(next);
}

AudioSourceEvent::AudioSourceEvent(AudioSource* audioSource) :
    audioSource(audioSource) {}

void AudioSourceEvent::perform()
{
    utils->audioSourceManager->addAudioSource(audioSource);

    audioSource->start(next);
}

GroupEvent::GroupEvent(std::vector<Event*> events) :
    events(events) {}

void GroupEvent::perform()
{
    for (Event* event : events)
    {
        utils->eventManager->addEvent(event);
    }
}

VariableGroupEvent::VariableGroupEvent(Variable* variable) :
    variable(variable) {}

void VariableGroupEvent::perform()
{
    ((GroupEvent*)variable->value)->perform();
}
