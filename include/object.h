#pragma once

#include "utils.h"

struct Object
{
    virtual ~Object();
};

struct Sync : public Object
{
    Sync();

    void start(double time);
    void repeat(double time);
    void stop();

    virtual double syncLength();

    double startTime = 0;
    double repeatTime = 0;

    bool enabled = false;

protected:
    virtual void finishStart();
    virtual void finishRepeat();
    virtual void finishStop();

    Utils* utils;

};

struct ValueObject : public Sync
{
    virtual double getValue();
};
