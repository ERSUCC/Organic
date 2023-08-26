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
    if (time - startTime >= length)
    {
        stop(time);

        return second.value;
    }

    return first.value + (second.value - first.value) * (time - startTime) / length;
}

Envelope::Envelope(double floor, double ceiling, unsigned int attack, unsigned int decay, double sustain, unsigned int release) :
    floor(floor), ceiling(ceiling), attack(attack), decay(decay), sustain(sustain), release(release) {}

double Envelope::getValue(double time)
{
    if (hold)
    {
        if (time - startTime < attack)
        {
            return floor.value + (ceiling.value - floor.value) * (time - startTime) / attack;
        }

        else if (time - startTime - attack < decay)
        {
            return ceiling.value - (ceiling.value - sustain.value) * (time - startTime - attack) / decay;
        }

        else
        {
            return sustain.value;
        }
    }

    if (time - releaseTime >= release)
    {
        stop(time);

        return floor.value;
    }

    return peak * (1 - (time - releaseTime) / release);
}

LFO::LFO(double floor, double ceiling, double rate) : floor(floor), ceiling(ceiling), rate(rate) {}

double LFO::getValue(double time)
{
    return floor.value + (ceiling.value - floor.value) * (-cos(Config::TWO_PI * (time - startTime) / rate) / 2 + 0.5);
}