#pragma once

#include "utils.h"

struct Object
{
    virtual ~Object();
};

struct Sync : public Object
{
    Sync();

    void start();
    void start(double time);
    void stop();

    virtual double syncLength();

    double startTime = 0;

    bool enabled = false;

protected:
    Utils* utils;

    virtual void finishStart();
    virtual void finishStop();

};

struct ValueObject : public Sync
{
    double getValue();

protected:
    virtual double getValueUnchecked() = 0;

};

struct Variable : public ValueObject
{
    ValueObject* value;

protected:
    void finishStart() override;
    void finishStop() override;

    double getValueUnchecked() override;

};