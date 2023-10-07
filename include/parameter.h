#pragma once

#include <unordered_set>
#include <vector>
#include <random>
#include <unordered_map>
#include <queue>

#include "config.h"

struct ParameterController;

struct Parameter
{
    Parameter(double value);
    Parameter(double value, ParameterController* source);

    double value;

    ParameterController* source;

    bool connected = false;
};

struct ParameterController
{
    friend struct ControllerGroup;
    friend struct ControllerManager;

    ParameterController(bool repeat);
    
    virtual void start(double time);
    void update(double time);
    void stop(double time);

    virtual double getValue(double time) = 0;

    double startTime;

protected:
    bool running = false;
    bool repeat;

private:
    std::unordered_set<Parameter*> connectedParameters;

};

struct ControllerGroup : public ParameterController
{
    enum Order
    {
        Forwards,
        Backwards,
        PingPong,
        Random
    };

    ControllerGroup(bool repeat, std::vector<ParameterController*> controllers, Order order);

    void start(double time) override;

    double getValue(double time) override;

private:
    std::vector<ParameterController*> controllers;

    Order order;

    int current = 0;
    int direction = 1;
    int last = -1;

    std::uniform_int_distribution<> udist;

};

struct ControllerManager
{
    void connectParameter(ParameterController* controller, Parameter* parameter);
    void disconnectParameter(ParameterController* controller, Parameter* parameter);

    void updateControllers(double time);

private:
    void orderControllers();

    std::vector<ParameterController*> controllers;

};

struct Sweep : public ParameterController
{
    Sweep(bool repeat, double first, double second, double length);

    double getValue(double time) override;

    Parameter first;
    Parameter second;
    Parameter length;
};

struct LFO : public ParameterController
{
    LFO(bool repeat, double floor, double ceiling, double rate);

    double getValue(double time) override;

    Parameter floor;
    Parameter ceiling;

    double rate;
};