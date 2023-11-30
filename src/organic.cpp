#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <queue>
#include <random>

#include "../include/RtAudio.h"

#include "../include/audiosource.h"
#include "../include/interpret.h"
#include "../include/utils.h"
#include "../include/effect.h"
#include "../include/event.h"
#include "../include/parameter.h"

struct AudioData
{
    std::vector<AudioSource*> sources;

    Utils* utils;
};

void rtAudioError(RtAudioErrorType type, const std::string& message)
{
    Utils::error(message);
}

int processAudio(void* output, void* input, unsigned int frames, double streamTime, RtAudioStreamStatus status, void* userData)
{
    if (status == RTAUDIO_INPUT_OVERFLOW)
    {
        Utils::warning("Stream overflow detected.");
    }

    if (status == RTAUDIO_OUTPUT_UNDERFLOW)
    {
        Utils::warning("Stream underflow detected.");
    }

    double* buffer = (double*)output;

    AudioData* data = (AudioData*)userData;

    std::fill(buffer, buffer + frames * data->utils->channels, 0);

    for (AudioSource* source : data->sources)
    {
        source->fillBuffer(buffer, frames);
    }

    for (int i = 0; i < frames * data->utils->channels; i += data->utils->channels)
    {
        buffer[i] *= data->utils->volume;

        if (data->utils->channels == 2)
        {
            buffer[i + 1] *= data->utils->volume;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        Utils::error("Not enough arguments specified.");
    }

    if (argc > 2)
    {
        Utils::error("Too many arguments specified.");
    }

    InterpreterResult interpreterResult = Interpreter::interpret(argv[1]);

    for (AudioSource* audioSource : interpreterResult.sources)
    {
        audioSource->start();
    }

    RtAudio audio(RtAudio::Api::UNSPECIFIED, rtAudioError);

    std::vector<unsigned int> ids = audio.getDeviceIds();

    if (ids.size() < 1)
    {
        Utils::error("No audio device detected.");
    }

    Utils* utils = Utils::get();

    AudioData data { interpreterResult.sources, utils };

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = utils->channels;

    unsigned int bufferFrames = utils->bufferLength;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, utils->sampleRate, &bufferFrames, &processAudio, (void*)&data))
    {
        Utils::error(audio.getErrorText());
    }

    if (audio.startStream())
    {
        if (audio.isStreamOpen())
        {
            audio.closeStream();
        }

        Utils::error(audio.getErrorText());
    }

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();

    while (true)
    {
        utils->time = (clock.now() - start).count() / 1000000.0;

        interpreterResult.eventQueue->performEvents();
    }

    if (audio.isStreamRunning())
    {
        audio.stopStream();
    }

    if (audio.isStreamOpen())
    {
        audio.closeStream();
    }

    return 0;
}