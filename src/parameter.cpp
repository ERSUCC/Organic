#include "../include/parameter.h"

Parameter::Parameter(double value) : value(value) {}
Parameter::Parameter(double value, ParameterController* source) : value(value), source(source) {}

Parameter::operator double()
{
    return value;
}

ParameterController::ParameterController(int repeats) : repeats(repeats) {}

void ParameterController::start()
{
    startTime = utils->time;
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
        Utils::error("Cannot connect two controllers to one parameter.");
    }

    if (std::find(controllers.begin(), controllers.end(), controller) == controllers.end())
    {
        controllers.push_back(controller);
    }

    parameter->connected = true;

    controller->connectedParameters.insert(parameter);

    if (std::find(controllers.begin(), controllers.end(), parameter->source) != controllers.end())
    {
        orderControllers();
    }
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
            Utils::error("Circular parameter reference.");
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

ControllerGroup::ControllerGroup(int repeats, std::vector<ParameterController*> controllers, OrderEnum order) :
    ParameterController(repeats), controllers(controllers), order(order) {}

ControllerGroup::Order::Order(OrderEnum order) : order(order) {}

void ControllerGroup::start()
{
    udist = std::uniform_int_distribution<>(0, controllers.size() - 1);

    if (order.order == OrderEnum::PingPong)
    {
        max_times = controllers.size() * 2 - 1;
    }

    else
    {
        max_times = controllers.size();
    }

    ParameterController::start();

    if (order.order == OrderEnum::Backwards)
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

        switch (order.order)
        {
            case OrderEnum::Forwards:
                current = (current + 1) % controllers.size();

                break;

            case OrderEnum::Backwards:
            {
                current -= 1;

                if (current < 0)
                {
                    current = controllers.size() - 1;
                }

                break;
            }

            case OrderEnum::PingPong:
            {
                if ((direction == -1 && current <= 0) || current >= controllers.size() - 1)
                {
                    direction *= -1;
                }

                current += direction;

                break;
            }

            case OrderEnum::Random:
            {
                current = udist(utils->rng);

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

Hold::Hold(double value, double length) :
    ParameterController(1), value(value, this), length(length, this) {}

double Hold::getValue()
{
    if (utils->time - startTime >= length)
    {
        stop();
    }

    return value;
}

Sweep::Sweep(int repeats, double from, double to, double length) :
    ParameterController(repeats), from(from, this), to(to, this), length(length, this) {}

double Sweep::getValue()
{
    if (utils->time - startTime >= length)
    {
        stop();

        return to;
    }

    return from + (to - from) * (utils->time - startTime) / length;
}

LFO::LFO(int repeats, double from, double to, double length) :
    ParameterController(repeats), from(from, this), to(to, this), length(length, this) {}

double LFO::getValue()
{
    if (utils->time >= startTime + length)
    {
        stop();
    }

    return from + (to - from) * (-cos(utils->twoPi * (utils->time - startTime) / length) / 2 + 0.5);
}