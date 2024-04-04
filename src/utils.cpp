#include "../include/utils.h"

Utils::Utils()
{
    rng = std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    audioSourceManager = new AudioSourceManager();
    eventManager = new EventManager();
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

void Utils::parseError(const std::string message, const std::string file, const int line, const int character)
{
    error("Parse error in \"" + file + "\" at line " + std::to_string(line) + " character " + std::to_string(character) + ": " + message);
}

void Utils::argumentError(const std::string message)
{
    error("Argument error: " + message);
}

void Utils::machineError(const std::string message, const std::string file)
{
    error("Virtual machine error in \"" + file + "\": " + message);
}
