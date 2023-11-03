#include "../include/parameter.h"

Parameter::Parameter(double value) : value(value) {}
Parameter::Parameter(double value, ParameterController* source) : value(value), source(source) {}

ParameterController::ParameterController(int repeats) : repeats(repeats) {}

void ParameterController::start()
{
    startTime = config->time;
    running = true;
}

void ParameterController::update()
{
    if (running)
    {
        double value = getValue();

        for (Parameter* parameter : connectedParameters)
        {
            parameter->value = value;
        }
    }
}

void ParameterController::stop()
{
    if (!repeats || ++times < repeats)
    {
        start();
    }

    else
    {
        running = false;
    }
}

void ControllerManager::connectParameter(ParameterController* controller, Parameter* parameter)
{
    if (parameter->connected)
    {
        // error, parameter can't be controlled by two controllers at once
    }

    if (std::find(controllers.begin(), controllers.end(), controller) == controllers.end())
    {
        controllers.push_back(controller);
    }

    parameter->connected = true;

    controller->connectedParameters.insert(parameter);

    orderControllers();
}

void ControllerManager::disconnectParameter(ParameterController* controller, Parameter* parameter)
{
    parameter->connected = false;

    controller->connectedParameters.erase(parameter);

    if (controller->connectedParameters.size() == 0)
    {
        controllers.erase(std::find(controllers.begin(), controllers.end(), controller));
    }
}

void ControllerManager::updateControllers()
{
    for (ParameterController* controller : controllers)
    {
        controller->update();
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

ControllerGroup::ControllerGroup(int repeats, std::vector<ParameterController*> controllers, Order order) :
    ParameterController(repeats), controllers(controllers), order(order)
{
    udist = std::uniform_int_distribution<>(0, controllers.size() - 1);

    if (order == Order::PingPong)
    {
        max_times = controllers.size() * 2 - 1;
    }

    else
    {
        max_times = controllers.size();
    }
}

void ControllerGroup::start()
{
    ParameterController::start();

    if (order == Order::Backwards)
    {
        current = controllers.size() - 1;
    }

    else
    {
        current = 0;
    }

    times = 0;

    controllers[current]->start();
}

double ControllerGroup::getValue()
{
    if (!controllers[current]->running)
    {
        last = current;

        switch (order)
        {
            case Order::Forwards:
                current = (current + 1) % controllers.size();

                break;

            case Order::Backwards:
            {
                current -= 1;

                if (current < 0)
                {
                    current = controllers.size() - 1;
                }

                break;
            }

            case Order::PingPong:
            {
                if ((direction == -1 && current <= 0) || current >= controllers.size() - 1)
                {
                    direction *= -1;
                }

                current += direction;

                break;
            }

            case Order::Random:
            {
                current = udist(config->rng);

                if (current == last)
                {
                    current = (current + 1) % controllers.size();
                }

                break;
            }
        }

        if (++times >= max_times)
        {
            stop();

            return controllers[controllers.size() - 1]->getValue();
        }

        controllers[current]->start();
    }

    return controllers[current]->getValue();
}

Value::Value(int repeats, double value, double length) :
    ParameterController(repeats), value(value), length(length) {}

double Value::getValue()
{
    if (config->time - startTime >= length.value)
    {
        stop();
    }

    return value.value;
}

Sweep::Sweep(int repeats, double first, double second, double length) :
    ParameterController(repeats), first(first, this), second(second, this), length(length, this) {}

double Sweep::getValue()
{
    if (config->time - startTime >= length.value)
    {
        stop();

        return second.value;
    }

    return first.value + (second.value - first.value) * (config->time - startTime) / length.value;
}

LFO::LFO(int repeats, double floor, double ceiling, double rate) :
    ParameterController(repeats), floor(floor, this), ceiling(ceiling, this), rate(rate) {}

double LFO::getValue()
{
    return floor.value + (ceiling.value - floor.value) * (-cos(config->twoPi * (config->time - startTime) / rate) / 2 + 0.5);
}