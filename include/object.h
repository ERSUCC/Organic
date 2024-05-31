#pragma once

#include "utils.h"

struct Sync
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

template <typename T> struct List : public ValueObject
{
    List(const std::vector<T*> objects) : objects(objects) {}

    const std::vector<T*> objects;
};

struct Variable : public ValueObject
{
    Variable(ValueObject* value);

    double syncLength() override;
    double getValue() override;

    ValueObject* value;

private:
    void finishStart() override;
    void finishStop() override;

};
