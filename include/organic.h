#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "../deps/AudioFile.h"
#include "../deps/RtAudio.h"

#include "flags.h"
#include "machine.h"
#include "parse.h"
#include "token.h"
#include "utils.h"

struct Organic
{
    Organic(const std::filesystem::path& path, const std::vector<std::string>& flags);

    void start();

private:
    void startPlayback();
    void startExport();

    int processAudio(void* output, unsigned int frames);

    Utils* utils;

    ProgramOptions options;

    Machine* machine;

    bool callbackActive = false;

};
