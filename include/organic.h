#pragma once

#include <string>
#include <vector>
#include <chrono>

#include "AudioFile.h"
#include "RtAudio.h"

#include "audiosource.h"
#include "event.h"
#include "interpret.h"
#include "utils.h"

struct AudioData
{
    std::vector<AudioSource*> audioSources;
    Utils* utils;
};

void rtAudioError(RtAudioErrorType type, const std::string& message);
int processAudio(void* output, void* input, unsigned int frames, double streamTime, RtAudioStreamStatus status, void* userData);

struct Organic
{
    Organic(const std::string program, const std::vector<std::string> flags);

    void start();
    void startPlayback();
    void startExport();

private:
    Utils* utils;

    std::vector<AudioSource*> audioSources;

    EventQueue* eventQueue;

    InterpreterOptions options;

};
