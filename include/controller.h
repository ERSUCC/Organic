#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <random>

#include "object.h"

struct ValueController : public ValueObject
{
    friend struct Sequence;

    ValueObject* repeats;

protected:
    void finishStop() override;

    int times = 0;

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

    std::vector<ValueObject*> controllers;

    Order* order;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

private:
    int current = 0;
    int direction = 1;
    int last = -1;
    int switches = 0;
    int max_switches;

    std::uniform_int_distribution<> udist;

};

struct Value : public ValueController
{
    Value(double value);

    double value;

protected:
    double getValueUnchecked() override;

};

struct Hold : public ValueController
{
    Hold();

    double syncLength() override;

    ValueObject* value;
    ValueObject* length;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

};

struct Sweep : public ValueController
{
    double syncLength() override;

    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

};

struct LFO : public ValueController
{
    double syncLength() override;

    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

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

    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

    Type* type;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

private:
    double current;
    double next = 0;

    bool first = true;

};
