#pragma once

#include <exception>
#include <string>

#include "location.h"

struct OrganicException : public std::exception
{
    OrganicException(const std::string preamble, const std::string message);

    const char* what() const noexcept override;

    bool operator==(const OrganicException& other) const;

    const std::string message;

private:
    const std::string error;

};

struct OrganicArgumentException : public OrganicException
{
    OrganicArgumentException(const std::string message);
};

struct OrganicFileException : public OrganicException
{
    OrganicFileException(const std::string message);
};

struct OrganicAudioException : public OrganicException
{
    OrganicAudioException(const std::string message);
};

struct OrganicParseException : public OrganicException
{
    OrganicParseException(const std::string message, const SourceLocation& location);

    const SourceLocation location;
};
