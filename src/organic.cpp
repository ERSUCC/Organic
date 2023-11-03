#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <queue>
#include <random>

#include "../include/RtAudio.h"

#include "../include/config.h"
#include "../include/audiosource.h"
#include "../include/parameter.h"
#include "../include/event.h"
#include "../include/effect.h"

struct AudioData
{
    std::vector<AudioSource*> sources;

    Config* config;
};

void error(const std::string& message)
{
    std::cout << message << "\n";

    exit(0);
}

void rtAudioError(RtAudioErrorType type, const std::string& message)
{
    error(message);
}

int processAudio(void* output, void* input, unsigned int frames, double streamTime, RtAudioStreamStatus status, void* userData)
{
    if (status == RTAUDIO_INPUT_OVERFLOW)
    {
        std::cout << "Error: Stream overflow detected.\n";
    }

    if (status == RTAUDIO_OUTPUT_UNDERFLOW)
    {
        std::cout << "Error: Stream underflow detected.\n";
    }

    double* buffer = (double*)output;

    AudioData* data = (AudioData*)userData;

    std::fill(buffer, buffer + frames * data->config->channels, 0);

    for (AudioSource* source : data->sources)
    {
        source->fillBuffer(buffer, frames);
    }

    for (int i = 0; i < frames * data->config->channels; i += data->config->channels)
    {
        buffer[i] *= data->config->volume;

        if (data->config->channels == 2)
        {
            buffer[i + 1] *= data->config->volume;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    RtAudio audio(RtAudio::Api::UNSPECIFIED, rtAudioError);

    std::vector<unsigned int> ids = audio.getDeviceIds();

    if (ids.size() < 1)
    {
        error("Error: No audio device detected.");
    }

    Config* config = Config::get();

    AudioData data;

    data.config = config;

    ControllerManager* controllerManager = new ControllerManager();

    EventQueue* eventQueue = new EventQueue();

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = config->channels;

    unsigned int bufferFrames = config->bufferLength;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, config->sampleRate, &bufferFrames, &processAudio, (void*)&data))
    {
        error(audio.getErrorText());
    }

    if (audio.startStream())
    {
        if (audio.isStreamOpen())
        {
            audio.closeStream();
        }

        error(audio.getErrorText());
    }

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();

    double time = 0;

    while (true)
    {
        time = (clock.now() - start).count() / 1000000.0;

        config->time = time;

        controllerManager->updateControllers();
        eventQueue->performEvents();
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