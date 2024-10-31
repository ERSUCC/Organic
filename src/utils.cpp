#include "../include/utils.h"

Utils::Utils()
{
    const unsigned int i = 1;

    if (*reinterpret_cast<const unsigned char*>(&i) == 1)
    {
        littleEndian = true;
    }

    e = exp(1);

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

void Utils::parseWarning(const std::string message, const SourceLocation location)
{
    warning("Parse warning in \"" + location.path + "\" at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t" + message);
}

void Utils::error(const std::string message)
{
    std::cout << message << "\n";

    exit(1);
}

void Utils::argumentError(const std::string message)
{
    error("Argument error:\n\t" + message);
}

void Utils::parseError(const std::string message, const SourceLocation location)
{
    error("Parse error in \"" + location.path + "\" at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t" + message);
}

void Utils::parseError(const std::string message, const std::string file, const unsigned int line, const unsigned int character)
{
    error("Parse error in \"" + file + "\" at line " + std::to_string(line) + " character " + std::to_string(character) + ":\n\t" + message);
}

void Utils::machineError(const std::string message, const std::string file)
{
    error("Virtual machine error in \"" + file + "\":\n\t" + message);
}
