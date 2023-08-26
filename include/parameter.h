#pragma once

#include <unordered_set>

struct Parameter
{
    Parameter(double value);

    double value;

    bool connected = false;
};

class ParameterController
{

public:
    void connectParameter(Parameter* parameter);
    void disconnectParameter(Parameter* parameter);

    void start(double time);
    void update(double time);
    void stop(double time);

    virtual double getValue(double time) = 0;

protected:
    double startTime;

    bool running;

    std::unordered_set<Parameter*> connectedParameters;

};

class Sweep : public ParameterController
{

public:
    Sweep(double first, double second, double length);

    double getValue(double time) override;

private:
    double first;
    double second;
    double length;

};

class Envelope : public ParameterController
{

public:
    Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release, double floor, double ceiling);

    double getValue(double time) override;

private:
    double attack;
    double decay;
    double sustain;
    double release;
    double floor;
    double ceiling;
    double peak;

    bool hold;

    double releaseTime;

};