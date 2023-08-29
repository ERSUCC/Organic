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

void ControllerManager::addController(ParameterController* controller)
{
    controllers.push_back(controller);
}

void ControllerManager::removeController(ParameterController* controller)
{
    controllers.erase(std::find(controllers.begin(), controllers.end(), controller));
}

void ControllerManager::connectParameter(ParameterController* controller, Parameter* parameter)
{
    controller->connectParameter(parameter);
}

void ControllerManager::disconnectParameter(ParameterController* controller, Parameter* parameter)
{
    controller->disconnectParameter(parameter);
}

void ControllerManager::updateControllers(double time)
{
    for (ParameterController* controller : controllers)
    {
        controller->update(time);
    }
}

Sweep::Sweep(double first, double second, double length) : first(first), second(second), length(length) {}

double Sweep::getValue(double time)
{
    if (time - startTime >= length.value)
    {
        stop(time);

        return second.value;
    }

    return first.value + (second.value - first.value) * (time - startTime) / length.value;
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

FiniteSequence::FiniteSequence(std::vector<double> values, Order order) : values(values), order(order)
{
    udist = std::uniform_int_distribution<>(0, values.size() - 1);

    if (order == Order::Backwards)
    {
        current = values.size() - 1;
    }
}

double FiniteSequence::getValue(double time)
{
    return values[current];
}

void FiniteSequence::next(double time)
{
    switch (order)
    {
        case Forwards:
            current = (current + 1) % values.size();

            break;

        case Backwards:
        {
            current -= 1;

            if (current < 0)
            {
                current = values.size() - 1;
            }

            break;
        }

        case PingPong:
        {
            if ((direction == -1 && current <= 0) || current >= values.size() - 1)
            {
                direction *= -1;
            }

            current += direction;

            break;
        }

        case Random:
            current = udist(Config::RNG);

            break;
    }
}