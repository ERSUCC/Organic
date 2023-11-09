#pragma once

#include <unordered_set>
#include <vector>
#include <random>
#include <unordered_map>
#include <queue>

#include "utils.h"

struct ParameterController;

struct Parameter : public Object
{
    Parameter(double value);
    Parameter(double value, ParameterController* source);

    operator double();

    double value;

    ParameterController* source;

    bool connected = false;
};

struct ParameterController : public Sync, public Object
{
    friend struct ControllerGroup;
    friend struct ControllerManager;

    ParameterController(int repeats);
    
    virtual void start();
    void update();
    void stop();

    virtual double getValue() = 0;

    double startTime;

    Parameter repeats;

protected:
    bool running = false;

    int times = 0;

private:
    std::unordered_set<Parameter*> connectedParameters;

};

struct ControllerManager
{
    void connectParameter(ParameterController* controller, Parameter* parameter);
    void disconnectParameter(ParameterController* controller, Parameter* parameter);

    void updateControllers();

private:
    void orderControllers();

    std::vector<ParameterController*> controllers;

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

    ControllerGroup(int repeats, std::vector<ParameterController*> controllers, OrderEnum order);

    void start() override;

    double getValue() override;

    std::vector<ParameterController*> controllers;

    Order order;

private:
    int current = 0;
    int direction = 1;
    int last = -1;
    int times = 0;
    int max_times;

    std::uniform_int_distribution<> udist;

};

struct Hold : public ParameterController
{
    Hold(double value, double length);

    double getValue() override;

    Parameter value;
    Parameter length;
};

struct Sweep : public ParameterController
{
    Sweep(int repeats, double from, double to, double length);

    double getValue() override;

    Parameter from;
    Parameter to;
    Parameter length;
};

struct LFO : public ParameterController
{
    LFO(int repeats, double from, double to, double length);

    double getValue() override;

    Parameter from;
    Parameter to;
    Parameter length;
};