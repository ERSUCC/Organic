#pragma once

#include "audiosource.h"
#include "management.h"
#include "object.h"

struct Event : public Sync, public EventTrait
{
    Event();

    bool ready() override;
    bool getNext() override;

    double next;
};

struct AssignEvent : public Event
{
    AssignEvent(Variable* variable, ValueObject* value);

    void perform() override;

private:
    Variable* variable;
    ValueObject* value;

};

struct CopyEvent : public Event
{
    CopyEvent(Variable* from, Variable* to);

    void perform() override;

private:
    Variable* from;
    Variable* to;

};

struct AudioSourceEvent : public Event
{
    AudioSourceEvent(AudioSource* audioSource);

    void perform() override;

private:
    AudioSource* audioSource;

};

struct GroupEvent : public Event
{
    GroupEvent(std::vector<Event*> events);

    void perform() override;

private:
    std::vector<Event*> events;

};

struct VariableGroupEvent : public Event
{
    VariableGroupEvent(Variable* variable);

    void perform() override;

private:
    Variable* variable;

};
