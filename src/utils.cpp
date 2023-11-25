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

void ValueObject::start()
{
    startTime = utils->time;

    enabled = true;

    finishStart();
}

void ValueObject::stop()
{
    enabled = false;

    finishStop();
}

double ValueObject::getValue()
{
    if (enabled)
    {
        return getValueUnchecked();
    }

    return 0;
}

void ValueObject::finishStart() {}
void ValueObject::finishStop() {}

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