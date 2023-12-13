#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>

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
    Organic(std::vector<AudioSource*> audioSources, EventQueue* eventQueue, InterpreterOptions options);

    static void init(const char* program, std::vector<const char*> flags);

    void start();
    void startTest();
    void startPlayback();

private:
    Utils* utils;

    std::vector<AudioSource*> audioSources;

    EventQueue* eventQueue;

    InterpreterOptions options;

};
