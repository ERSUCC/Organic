#pragma once

#include <unordered_set>
#include <vector>
#include <random>

#include "object.h"

struct ValueController : public ValueObject {};

struct ValueCombination : public ValueController
{
    ValueCombination(ValueObject* value1, ValueObject* value2);

    double syncLength() override;
    double getValue() override;

    ValueObject* value1;
    ValueObject* value2;

protected:
    void finishStart() override;
    void finishStop() override;

    virtual double getValueInternal() = 0;

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

struct Sequence : public ValueController
{
    enum OrderEnum
    {
        Forwards,
        Backwards,
        PingPong,
        Random
    };

    struct Order : public Object
    {
        Order(OrderEnum order);
        
        OrderEnum order;
    };

    Sequence(List<ValueObject>* controllers, Order* order);

    double syncLength() override;
    double getValue() override;

    List<ValueObject>* controllers;

    std::unordered_set<int> chosen;

    Order* order;

protected:
    void finishStart() override;
    void finishRepeat() override;

private:
    int current = 0;
    int direction = 1;
    int last = -1;
    int switches = 0;
    int max_switches;

    std::uniform_int_distribution<> udist;

};

struct Repeat : public ValueController
{
    Repeat(ValueObject* value, ValueObject* repeats);

    double syncLength() override;
    double getValue() override;

    ValueObject* value;
    ValueObject* repeats;

protected:
    void finishStart() override;
    void finishRepeat() override;

private:
    int times = 0;

};

struct Value : public ValueController
{
    Value(double value);

    double getValue() override;

    double value;
};

struct Hold : public ValueController
{
    Hold(ValueObject* value, ValueObject* length);

    double syncLength() override;
    double getValue() override;

    ValueObject* value;
    ValueObject* length;

protected:
    void finishStart() override;

};

struct Sweep : public ValueController
{
    Sweep(ValueObject* from, ValueObject* to, ValueObject* length);

    double syncLength() override;
    double getValue() override;

    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

protected:
    void finishStart() override;

};

struct LFO : public ValueController
{
    LFO(ValueObject* from, ValueObject* to, ValueObject* length);

    double syncLength() override;
    double getValue() override;

    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

protected:
    void finishStart() override;

};

struct Random : public ValueController
{
    enum TypeEnum
    {
        Step,
        Linear
    };

    struct Type : public Object
    {
        Type(TypeEnum type);
        
        TypeEnum type;
    };

    Random(ValueObject* from, ValueObject* to, ValueObject* length, Type* type);

    double syncLength() override;
    double getValue() override;

    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

    Type* type;

protected:
    void finishStart() override;

private:
    double current;
    double next = 0;

    bool first = true;

};
