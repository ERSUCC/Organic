#include "../include/config.h"

Config::Config()
{
    rng = std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

Config* Config::get()
{
    static Config* instance;

    if (!instance)
    {
        instance = new Config();
    }

    return instance;
}

Sync::Sync()
{
    config = Config::get();
}