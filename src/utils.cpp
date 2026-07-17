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

void Utils::printInfo()
{
    Utils* utils = Utils::get();

    std::cout << "Organic v0.1.0" << "\n";
    std::cout << "Channels: " << utils->channels << "\n";
    std::cout << "Sample Rate: " << utils->sampleRate << "\n";
    std::cout << "Buffer Length: " << utils->bufferLength << "\n";
    std::cout << "Random Seed: " << utils->seed << "\n";

    utils->firstPrint = false;
}

void Utils::setWarnLevel(const WarnLevel& level)
{
    Utils::get()->warnLevel = level;
}

void Utils::parseWarning(const std::string& message, const SourceLocation& location)
{
    const Utils* utils = Utils::get();

    if (utils->warnLevel == WarnLevel::Suppress)
    {
        return;
    }

    if (utils->warnLevel == WarnLevel::Error)
    {
        throw OrganicParseException(message, location);
    }

    printWarning("Parse warning in " + location.source->description() + " at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n    " + message);
}

void Utils::printWarning(const std::string& text)
{
    Utils* utils = Utils::get();

    if (!utils->firstPrint)
    {
        std::cout << "\n";
    }

    else
    {
        utils->firstPrint = false;
    }

    std::cout << "\x1b[38;2;210;210;50m" << text << "\x1b[49;;m\n";
}

void Utils::printError(const std::string& text)
{
    Utils* utils = Utils::get();

    if (!utils->firstPrint)
    {
        std::cout << "\n";
    }

    else
    {
        utils->firstPrint = false;
    }

    std::cout << "\x1b[38;2;255;128;128m" << text << "\x1b[49;;m\n";
}

void Utils::setSeed(const std::optional<size_t>& seed)
{
    this->seed = seed.value_or(std::chrono::high_resolution_clock::now().time_since_epoch().count());

    rng = std::mt19937_64(this->seed);
}

Profiler::Profiler(const double frequency) :
    frequency(frequency)
{
    reset();
}

void Profiler::start()
{
    startTime = clock.now();
}

void Profiler::report()
{
    static std::mutex lock;

    elapsed += clock.now() - startTime;
    count++;

    if ((clock.now() - reportTime).count() >= frequency * 1e9)
    {
        lock.lock();

        std::cout << elapsed.count() / count << "\n";

        lock.unlock();

        reset();
    }
}

void Profiler::reset()
{
    reportTime = clock.now();
    elapsed = std::chrono::nanoseconds(0);
    count = 0;
}
