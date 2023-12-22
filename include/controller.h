#pragma once

#include <unordered_set>
#include <vector>
#include <random>

#include "object.h"

struct ValueController : public ValueObject
{
    friend struct Sequence;
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

    double syncLength() override;
    double getValue() override;

    std::vector<ValueObject*> controllers;

    std::unordered_set<int> chosen;

    Order* order;

protected:
    void finishStart() override;
    void finishStop() override;

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
    double syncLength() override;
    double getStartTime() override;
    double getValue() override;

    ValueObject* value;
    ValueObject* repeats;

protected:
    void finishStart() override;
    void finishStop() override;

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
    double syncLength() override;
    double getValue() override;

    ValueObject* value;
    ValueObject* length;

protected:
    void finishStart() override;

};

struct Sweep : public ValueController
{
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
