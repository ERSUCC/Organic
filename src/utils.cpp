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

Sync::Sync()
{
    utils = Utils::get();
}

double Object::getValue()
{
    return 0;
}

double Variable::getValue()
{
    return value->getValue();
}