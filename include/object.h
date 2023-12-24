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
    virtual double getStartTime();

    double startTime = 0;

    bool enabled = false;

protected:
    virtual void finishStart();
    virtual void finishStop();

    Utils* utils;

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

struct ValueCombination : public ValueObject
{
    ValueCombination(ValueObject* value1, ValueObject* value2);

    double syncLength() override;

    ValueObject* value1;
    ValueObject* value2;

protected:
    void finishStart() override;
    void finishStop() override;

};

struct ValueAdd : public ValueCombination
{
    ValueAdd(ValueObject* value1, ValueObject* value2);

    double getValue() override;
};

struct ValueSubtract : public ValueCombination
{
    ValueSubtract(ValueObject* value1, ValueObject* value2);

    double getValue() override;
};

struct ValueMultiply : public ValueCombination
{
    ValueMultiply(ValueObject* value1, ValueObject* value2);

    double getValue() override;
};

struct ValueDivide : public ValueCombination
{
    ValueDivide(ValueObject* value1, ValueObject* value2);

    double getValue() override;
};
