#include "../include/utils.h"

Utils::Utils()
{
    rng = std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

Utils* Utils::get()
{
    static Utils* instance;

    if (!instance)
    {
        instance = new Utils();
    }

    return instance;
}

void Utils::warning(const std::string message)
{
    std::cout << message << "\n";
}

void Utils::error(const std::string message)
{
    std::cout << message << "\n";

    exit(1);
}

AssignableObject::~AssignableObject() {}

Sync::Sync()
{
    utils = Utils::get();
}

void Sync::start()
{
    start(utils->time);
}

void Sync::start(double time)
{
    startTime = time;

    enabled = true;

    finishStart();
}

void Sync::stop()
{
    enabled = false;

    finishStop();
}

double Sync::syncLength()
{
    return 0;
}

void Sync::finishStart() {}
void Sync::finishStop() {}

double ValueObject::getValue()
{
    if (enabled)
    {
        return getValueUnchecked();
    }

    return 0;
}

double Variable::getValueUnchecked()
{
    return value->getValue();
}

void Variable::finishStart()
{
    value->start();
}

void Variable::finishStop()
{
    value->stop();
}