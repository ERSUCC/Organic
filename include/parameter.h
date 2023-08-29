#pragma once

#include <unordered_set>
#include <vector>
#include <random>

#include "config.h"

struct Parameter
{
    Parameter(double value);

    double value;

    bool connected = false;
};

struct ParameterController
{
    void connectParameter(Parameter* parameter);
    void disconnectParameter(Parameter* parameter);

    void start(double time);
    void update(double time);
    void stop(double time);

    virtual double getValue(double time) = 0;

    double startTime;

    bool running;

    std::unordered_set<Parameter*> connectedParameters;
};

struct ControllerManager
{
    void addController(ParameterController* controller);
    void removeController(ParameterController* controller);

    void connectParameter(ParameterController* controller, Parameter* parameter);
    void disconnectParameter(ParameterController* controller, Parameter* parameter);

    void updateControllers(double time);

    std::vector<ParameterController*> controllers;
};

struct Sweep : public ParameterController
{
    Sweep(double first, double second, double length);

    double getValue(double time) override;

    Parameter first;
    Parameter second;
    Parameter length;
};

struct Envelope : public ParameterController
{
    Envelope(double floor, double ceiling, unsigned int attack, unsigned int decay, double sustain, unsigned int release);

    double getValue(double time) override;

    Parameter floor;
    Parameter ceiling;
    Parameter sustain;

    double attack;
    double decay;
    double release;
    double peak;

    bool hold;

    double releaseTime;
};

struct LFO : public ParameterController
{
    LFO(double floor, double ceiling, double rate);

    double getValue(double time) override;

    Parameter floor;
    Parameter ceiling;

    double rate;
};

struct Sequence : public ParameterController
{
    virtual void next(double time) = 0;
};

struct FiniteSequence : public Sequence
{
    enum Order
    {
        Forwards,
        Backwards,
        PingPong,
        Random
    };

    FiniteSequence(std::vector<double> values, Order order);

    double getValue(double time) override;
    void next(double time) override;

    std::vector<double> values;

    int current = 0;
    int direction = 1;

    Order order;

    std::uniform_int_distribution<> udist;
};