#pragma once

#define _USE_MATH_DEFINES

#if defined(_WIN32)
    #include <math.h>
#else
    #include <cmath>
#endif

#include <chrono>
#include <exception>
#include <iostream>
#include <limits>
#include <random>
#include <string>

#include "location.h"
#include "path.h"

struct Utils
{
    static Utils* get();

    static void parseWarning(const std::string message, const SourceLocation& location);
    static void includeWarning(const std::string message, const SourceLocation& location);

    void setSeed(unsigned int seed);

    bool littleEndian = false;

    unsigned int sampleRate = 44100;
    unsigned int channels = 2;
    unsigned int bufferLength = 128;

    const double pi = M_PI;
    const double twoPi = M_PI * 2;
    const double infinity = std::numeric_limits<double>::infinity();

    double e;

    double volume = 0.25;
    double time = 0;

    std::mt19937 rng;

private:
    Utils();

    static Utils* instance;

    bool firstWarning = true;

};

struct OrganicException : public std::exception
{
    OrganicException(const std::string message);

    const char* what() const noexcept override;

    bool operator==(const OrganicException& other) const;

private:
    const std::string message;

};

struct OrganicArgumentException : public OrganicException
{
    OrganicArgumentException(const std::string message);
};

struct OrganicFileException : public OrganicException
{
    OrganicFileException(const std::string message);
};

struct OrganicParseException : public OrganicException
{
    OrganicParseException(const std::string message, const SourceLocation& location);

    const SourceLocation location;
};

struct OrganicIncludeException : public OrganicException
{
    OrganicIncludeException(const std::string message, const SourceLocation& location);

    const SourceLocation location;
};

struct OrganicMachineException : public OrganicException
{
    OrganicMachineException(const std::string message);
};

struct OrganicAudioException : public OrganicException
{
    OrganicAudioException(const std::string message);
};

struct Profiler
{
    Profiler(const double frequency);

    void start();
    void report();

private:
    void reset();

    const double frequency;

    std::chrono::high_resolution_clock clock;

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> reportTime;

    std::chrono::duration<std::chrono::high_resolution_clock::rep, std::chrono::high_resolution_clock::period> elapsed;

    size_t count;

};
