#include "../include/exception.h"

OrganicException::OrganicException(const std::string preamble, const std::string message) :
    message(message), preamble(preamble) {}

const char* OrganicException::what() const noexcept
{
    return (preamble + message).c_str();
}

bool OrganicException::operator==(const OrganicException& other) const
{
    return preamble == other.preamble && message == other.message;
}

OrganicArgumentException::OrganicArgumentException(const std::string message) :
    OrganicException("Argument error:\n\t", message) {}

OrganicFileException::OrganicFileException(const std::string message) :
    OrganicException("File error:\n\t", message) {}

OrganicAudioException::OrganicAudioException(const std::string message) :
    OrganicException("Audio error:\n\t", message) {}

OrganicParseException::OrganicParseException(const std::string message, const SourceLocation& location) :
    OrganicException("Parse error in \"" + location.source->path->string() + "\" at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t", message), location(location) {}

OrganicIncludeException::OrganicIncludeException(const std::string message, const SourceLocation& location) :
    OrganicException("Include error in \"" + location.source->path->string() + "\" at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t", message), location(location) {}
