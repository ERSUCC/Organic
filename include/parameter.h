#pragma once

#include <unordered_set>

#include "constants.h"

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

struct Sweep : public ParameterController
{
    Sweep(double first, double second, double length);

    double getValue(double time) override;

    Parameter first;
    Parameter second;

    double length;
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