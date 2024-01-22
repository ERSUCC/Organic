#pragma once

#include "utils.h"

struct Object
{
    Object();

    virtual ~Object();

protected:
    Utils* utils;

};

template <typename T> struct List : public Object
{
    List(const std::vector<T*> objects) : objects(objects) {}

    const std::vector<T*> objects;
};

struct Sync : public Object
{
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

};

struct ValueObject : public Sync
{
    virtual double getValue();
};

struct Variable : public ValueObject
{
    double syncLength() override;
    double getValue() override;

    ValueObject* value;

protected:
    void finishStart() override;
    void finishStop() override;

};
