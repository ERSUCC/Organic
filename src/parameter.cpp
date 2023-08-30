#include "../include/parameter.h"

#include <iostream>

Parameter::Parameter(double value) : value(value) {}
Parameter::Parameter(double value, ParameterController* source) : value(value), source(source) {}

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

    orderControllers();
}

void ControllerManager::removeController(ParameterController* controller)
{
    for (Parameter* parameter : controller->connectedParameters)
    {
        parameter->connected = false;
    }

    controllers.erase(std::find(controllers.begin(), controllers.end(), controller));
}

void ControllerManager::connectParameter(ParameterController* controller, Parameter* parameter)
{
    parameter->connected = true;

    controller->connectedParameters.insert(parameter);

    orderControllers();
}

void ControllerManager::disconnectParameter(ParameterController* controller, Parameter* parameter)
{
    parameter->connected = false;

    controller->connectedParameters.erase(parameter);
}

void ControllerManager::updateControllers(double time)
{
    for (ParameterController* controller : controllers)
    {
        controller->update(time);
    }
}

void ControllerManager::orderControllers()
{
    std::unordered_map<ParameterController*, int> indices;

    for (int i = 0; i < controllers.size(); i++)
    {
        indices.insert({ controllers[i], i });
    }

    std::vector<std::vector<int>> edges(controllers.size(), std::vector<int>());
    std::vector<int> degrees(edges.size());

    for (int i = 0; i < controllers.size(); i++)
    {
        for (Parameter* parameter : controllers[i]->connectedParameters)
        {
            if (parameter->source)
            {
                edges[i].push_back(indices[parameter->source]);
                degrees[indices[parameter->source]]++;
            }
        }
    }

    std::queue<int> queue;

    for (int i = 0; i < controllers.size(); i++)
    {
        if (!degrees[i])
        {
            queue.push(i);
        }
    }

    std::vector<ParameterController*> order;

    for (int i = 0; i < controllers.size(); i++)
    {
        if (queue.empty())
        {
            // error, circular parameter reference
        }

        int next = queue.front();

        queue.pop();

        order.push_back(controllers[next]);

        for (int adj : edges[next])
        {
            if (!--degrees[adj])
            {
                queue.push(adj);
            }
        }
    }

    controllers.clear();
    controllers.insert(controllers.begin(), order.begin(), order.end());
}

Sweep::Sweep(double first, double second, double length) : first(first, this), second(second, this), length(length, this) {}

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
    floor(floor, this), ceiling(ceiling, this), attack(attack), decay(decay), sustain(sustain, this), release(release) {}

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

LFO::LFO(double floor, double ceiling, double rate) : floor(floor, this), ceiling(ceiling, this), rate(rate) {}

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