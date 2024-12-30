#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

#include "../deps/AudioFile.h"
#include "../deps/RtAudio.h"

#include "flags.h"
#include "machine.h"
#include "parse.h"
#include "token.h"
#include "utils.h"

struct AudioData
{
    Utils* utils;
    Machine* machine;

    bool callbackActive = false;
};

void rtAudioError(RtAudioErrorType type, const std::string& message);
int processAudio(void* output, void* input, unsigned int frames, double streamTime, RtAudioStreamStatus status, void* userData);

struct Organic
{
    Organic(const std::filesystem::path& path, const std::vector<std::string>& flags);

    void start();

private:
    void startPlayback();
    void startExport();

    Utils* utils;

    ProgramOptions options;

    Machine* machine;

};
