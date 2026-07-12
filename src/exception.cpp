#include "../include/exception.h"

OrganicException::OrganicException(const std::string& preamble, const std::string& message) :
    message(message), error(preamble + message) {}

const char* OrganicException::what() const noexcept
{
    return error.c_str();
}

bool OrganicException::operator==(const OrganicException& other) const
{
    return error == other.error;
}

OrganicArgumentException::OrganicArgumentException(const std::string& message) :
    OrganicException("Argument error:\n    ", message) {}

OrganicFileException::OrganicFileException(const std::string& message) :
    OrganicException("File error:\n    ", message) {}

OrganicAudioException::OrganicAudioException(const std::string& message) :
    OrganicException("Audio error:\n    ", message) {}

OrganicParseException::OrganicParseException(const std::string& message, const SourceLocation& location) :
    OrganicException("Parse error in " + location.source->description() + " at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n    ", message), location(location) {}
