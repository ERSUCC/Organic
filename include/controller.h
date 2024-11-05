#pragma once

#include <random>
#include <unordered_set>
#include <vector>

#include "object.h"

struct ValueCombination : public ValueObject
{
    ValueCombination(ValueObject* value1, ValueObject* value2);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;

    virtual double getValueInternal() = 0;

    ValueObject* value1;
    ValueObject* value2;

};

struct ValueAdd : public ValueCombination
{
    ValueAdd(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;

};

struct ValueSubtract : public ValueCombination
{
    ValueSubtract(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;

};

struct ValueMultiply : public ValueCombination
{
    ValueMultiply(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;
    
};

struct ValueDivide : public ValueCombination
{
    ValueDivide(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;
    
};

struct ValuePower : public ValueCombination
{
    ValuePower(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;
    
};

struct ValueEquals : public ValueCombination
{
    ValueEquals(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;

};

struct ValueLess : public ValueCombination
{
    ValueLess(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;

};

struct ValueGreater : public ValueCombination
{
    ValueGreater(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;

};

struct ValueLessEqual : public ValueCombination
{
    ValueLessEqual(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;

};

struct ValueGreaterEqual : public ValueCombination
{
    ValueGreaterEqual(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal() override;

};

struct Sequence : public ValueObject
{
    enum OrderEnum
    {
        Forwards,
        Backwards,
        PingPong,
        Random
    };

    Sequence(ValueObject* controllers, ValueObject* order);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;
    void reinit() override;

private:
    ValueObject* controllers;
    ValueObject* order;

    std::uniform_int_distribution<> udist;

    std::unordered_set<unsigned int> chosen;

    int current = 0;
    int direction = 1;
    int last = -1;
    int switches = 0;
    int max_switches;

};

struct Repeat : public ValueObject
{
    Repeat(ValueObject* value, ValueObject* repeats);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;
    void reinit() override;

private:
    ValueObject* value;
    ValueObject* repeats;

    unsigned int times = 0;

};

struct Value : public ValueObject
{
    Value(double value);

    double getValue() override;

    double value;
};

struct Hold : public ValueObject
{
    Hold(ValueObject* value, ValueObject* length);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;

private:
    ValueObject* value;
    ValueObject* length;

};

struct Sweep : public ValueObject
{
    Sweep(ValueObject* from, ValueObject* to, ValueObject* length);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

};

struct LFO : public ValueObject
{
    LFO(ValueObject* from, ValueObject* to, ValueObject* length);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

};

struct Random : public ValueObject
{
    enum TypeEnum
    {
        Step,
        Linear
    };

    Random(ValueObject* from, ValueObject* to, ValueObject* length, ValueObject* type);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

    ValueObject* type;

    double current;
    double next = 0;

    bool first = true;

};

struct Limit : public ValueObject
{
    Limit(ValueObject* value, ValueObject* min, ValueObject* max);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;

private:
    ValueObject* value;
    ValueObject* min;
    ValueObject* max;

};

struct Trigger : public ValueObject
{
    Trigger(ValueObject* condition, ValueObject* value);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;

private:
    ValueObject* condition;
    ValueObject* value;

    bool triggered = false;

};

struct If : public ValueObject
{
    If(ValueObject* condition, ValueObject* trueValue, ValueObject* falseValue);

    double syncLength() override;
    double getValue() override;

protected:
    void init() override;

private:
    ValueObject* condition;
    ValueObject* trueValue;
    ValueObject* falseValue;

};
