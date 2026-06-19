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
    const Utils* utils = Utils::get();

    std::cout << "Organic v0.1.0" << "\n";
    std::cout << "Channels: " << utils->channels << "\n";
    std::cout << "Sample Rate: " << utils->sampleRate << "\n";
    std::cout << "Buffer Length: " << utils->bufferLength << "\n";
    std::cout << "Random Seed: " << utils->seed << "\n";
}

void Utils::includeWarning(const std::string message, const SourceLocation& location)
{
    if (!Utils::get()->firstWarning)
    {
        std::cout << "\n";
    }

    else
    {
        Utils::get()->firstWarning = false;
    }

    std::cout << "Include warning in " + location.source->description() + " at line " + std::to_string(location.line) + " character " + std::to_string(location.character) + ":\n\t" + message << "\n";
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
