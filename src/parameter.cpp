#include "../include/parameter.h"

Parameter::Parameter(double value) : value(value) {}

void ParameterController::connectParameter(Parameter* parameter)
{
    if (parameter->connected)
    {
        // error
    }

    parameter->connected = true;

    connectedParameters.insert(parameter);
}

void ParameterController::disconnectParameter(Parameter* parameter)
{
    if (!connectedParameters.count(parameter))
    {
        // error
    }

    parameter->connected = false;

    connectedParameters.erase(parameter);
}

void ParameterController::start(double time)
{
    startTime = time;
    running = true;
}

void ParameterController::update(double time)
{
    if (running)
    {
        double value = getValue(time);

        for (Parameter* parameter : connectedParameters)
        {
            parameter->value = value;
        }
    }
}

void ParameterController::stop(double time)
{
    running = false;
}

Sweep::Sweep(double first, double second, double length) : first(first), second(second), length(length) {}

double Sweep::getValue(double time)
{
    if (first < second)
    {
        return fmin(second, first + (second - first) * (time - startTime) / length);
    }

    return fmax(second, first + (second - first) * (time - startTime) / length);
}

Envelope::Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release, double floor, double ceiling) :
    attack(attack), decay(decay), sustain(sustain), release(release), floor(floor), ceiling(ceiling) {}

double Envelope::getValue(double time)
{
    if (hold)
    {
        if (time - startTime < attack)
        {
            return floor + (ceiling - floor) * (time - startTime) / attack;
        }

        else if (time - startTime - attack < decay)
        {
            return ceiling - (ceiling - sustain) * (time - startTime - attack) / decay;
        }

        else
        {
            return sustain;
        }
    }

    return fmax(floor, peak * (1 - (time - releaseTime) / release));
}