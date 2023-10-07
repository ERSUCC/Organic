#include "../include/parameter.h"

#include <iostream>

Parameter::Parameter(double value) : value(value) {}
Parameter::Parameter(double value, ParameterController* source) : value(value), source(source) {}

ParameterController::ParameterController(bool repeat) : repeat(repeat) {}

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
    if (repeat)
    {
        startTime = time;
    }

    else
    {
        running = false;
    }
}

ControllerGroup::ControllerGroup(bool repeat, std::vector<ParameterController*> controllers) :
    ParameterController(repeat), controllers(controllers) {}

double ControllerGroup::getValue(double time)
{
    if (!controllers[current]->running && ++current >= controllers.size())
    {
        stop(time);

        current -= 1;
    }

    return controllers[current]->getValue(time);
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
    if (parameter->connected)
    {
        // error, parameter can't be controlled by two controllers at once
    }

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

Sweep::Sweep(bool repeat, double first, double second, double length) :
    ParameterController(repeat), first(first, this), second(second, this), length(length, this) {}

double Sweep::getValue(double time)
{
    if (time - startTime >= length.value)
    {
        stop(time);

        return second.value;
    }

    return first.value + (second.value - first.value) * (time - startTime) / length.value;
}

LFO::LFO(bool repeat, double floor, double ceiling, double rate) :
    ParameterController(repeat), floor(floor, this), ceiling(ceiling, this), rate(rate) {}

double LFO::getValue(double time)
{
    return floor.value + (ceiling.value - floor.value) * (-cos(Config::TWO_PI * (time - startTime) / rate) / 2 + 0.5);
}

Sequence::Sequence(bool repeat) : ParameterController(repeat) {}

FiniteSequence::FiniteSequence(bool repeat, std::vector<double> values, Order order) :
    Sequence(repeat), values(values), order(order)
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
        {
            current = udist(Config::RNG);

            if (current == last)
            {
                current = (current + 1) % values.size();
            }

            break;
        }
    }

    last = current;
}