#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "../deps/AudioFile.h"
#include "../deps/RtAudio.h"

#include "interpret.h"
#include "machine.h"
#include "utils.h"

void rtAudioError(RtAudioErrorType type, const std::string& message);
int processAudio(void* output, void* input, unsigned int frames, double streamTime, RtAudioStreamStatus status, void* userData);

struct Organic
{
    Organic(const std::string program, const std::vector<std::string> flags);

    void start();

private:
    void startPlayback();
    void startExport();

    Utils* utils;

    InterpreterOptions options;

    Machine* machine;

};
