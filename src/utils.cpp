#include "../include/utils.h"

Utils* Utils::get()
{
    static Utils* instance;

    if (!instance)
    {
        instance = new Utils();
    }

    return instance;
}

void Utils::parseWarning(const std::string message, const SourceLocation& location)
{
    std::cout << "Parse warning in \"" + location.path->string() + "\" at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t" + message << "\n";
}

void Utils::includeWarning(const std::string message, const SourceLocation& location)
{
    std::cout << "Include warning in \"" + location.path->string() + "\" at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t" + message << "\n";
}

void Utils::setSeed(unsigned int seed)
{
    rng = std::mt19937(seed);
}

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

OrganicException::OrganicException(const std::string message) :
    message(message) {}

const char* OrganicException::what() const
{
    return message.c_str();
}

OrganicArgumentException::OrganicArgumentException(const std::string message) :
    OrganicException("Argument error:\n\t" + message) {}

OrganicFileException::OrganicFileException(const std::string message) :
    OrganicException("File error:\n\t" + message) {}

OrganicParseException::OrganicParseException(const std::string message, const SourceLocation& location) :
    OrganicException("Parse error in \"" + location.path->string() + "\" at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t" + message), location(location) {}

OrganicIncludeException::OrganicIncludeException(const std::string message, const SourceLocation& location) :
    OrganicException("Include error in \"" + location.path->string() + "\" at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t" + message), location(location) {}

OrganicMachineException::OrganicMachineException(const std::string message) :
    OrganicException("Virtual machine error:\n\t" + message) {}

OrganicAudioException::OrganicAudioException(const std::string message) :
    OrganicException("Audio error:\n\t" + message) {}
