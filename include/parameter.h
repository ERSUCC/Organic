#pragma once

#include <unordered_set>
#include <vector>
#include <random>
#include <unordered_map>
#include <queue>

#include "utils.h"

struct ParameterController : public Sync, public Object
{
    friend struct ControllerGroup;
    friend struct ControllerManager;

    virtual void start();
    void stop();

    double startTime;

    ParameterController* repeats;

protected:
    bool running = false;

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

    struct Order : public Object
    {
        Order(OrderEnum order);
        
        OrderEnum order;
    };

    void start() override;

    double getValue() override;

    std::vector<ParameterController*> controllers;

    Order* order;

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

    double getValue() override;

    double value;
};

struct Hold : public ParameterController
{
    double getValue() override;

    ParameterController* value = new ParameterController();
    ParameterController* length = new ParameterController();
};

struct Sweep : public ParameterController
{
    double getValue() override;

    ParameterController* from = new ParameterController();
    ParameterController* to = new ParameterController();
    ParameterController* length = new ParameterController();
};

struct LFO : public ParameterController
{
    double getValue() override;

    ParameterController* from = new ParameterController();
    ParameterController* to = new ParameterController();
    ParameterController* length = new ParameterController();
};