#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <random>

#include "utils.h"

struct ParameterController : public ValueObject
{
    friend struct ControllerGroup;

    ValueObject* repeats;

protected:
    void finishStop() override;

    int times = 0;

};

struct ControllerGroup : public ParameterController
{
    enum OrderEnum
    {
        Forwards,
        Backwards,
        PingPong,
        Random
    };

    struct Order : public AssignableObject
    {
        Order(OrderEnum order);
        
        OrderEnum order;
    };

    std::vector<ValueObject*> controllers;

    Order* order;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

private:
    int current = 0;
    int direction = 1;
    int last = -1;
    int times = 0;
    int max_times;

    std::uniform_int_distribution<> udist;

};

struct Value : public ParameterController
{
    Value(double value);

    double value;

protected:
    double getValueUnchecked() override;

};

struct Hold : public ParameterController
{
    ValueObject* value;
    ValueObject* length;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

};

struct Sweep : public ParameterController
{
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

};

struct LFO : public ParameterController
{
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

protected:
    void finishStart() override;

    double getValueUnchecked() override;

};