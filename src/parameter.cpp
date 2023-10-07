#include "../include/parameter.h"

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
        start(time);
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

ControllerGroup::ControllerGroup(bool repeat, std::vector<ParameterController*> controllers, Order order) :
    ParameterController(repeat), controllers(controllers), order(order)
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

void ControllerGroup::start(double time)
{
    ParameterController::start(time);

    if (order == Order::Backwards)
    {
        current = controllers.size() - 1;
    }

    else
    {
        current = 0;
    }

    times = 0;

    controllers[current]->start(time);
}

double ControllerGroup::getValue(double time)
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
                current = udist(Config::RNG);

                if (current == last)
                {
                    current = (current + 1) % controllers.size();
                }

                break;
            }
        }

        if (++times >= max_times)
        {
            stop(time);

            return controllers[controllers.size() - 1]->getValue(time);
        }

        controllers[current]->start(time);
    }

    return controllers[current]->getValue(time);
}

Value::Value(bool repeat, double value, double length) :
    ParameterController(repeat), value(value), length(length) {}

double Value::getValue(double time)
{
    if (time - startTime >= length.value)
    {
        stop(time);
    }

    return value.value;
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