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

    for (int i = 0; i < frames * Config::CHANNELS; i++)
    {
        buffer[i] = 0;
    }

    for (AudioSource* source : data->sources)
    {
        source->fillBuffer(buffer, frames);
    }

    for (int i = 0; i < frames * Config::CHANNELS; i += Config::CHANNELS)
    {
        buffer[i] *= Config::MASTER_VOLUME;

        if (Config::CHANNELS == 2)
        {
            buffer[i + 1] *= Config::MASTER_VOLUME;
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

    AudioData data;

    ControllerManager* controllerManager = new ControllerManager();

    EventQueue* eventQueue = new EventQueue();

    Triangle* test = new Triangle(1, 0, 0);

    data.sources.push_back(test);

    test->addEffect(new Delay(1, 250, 0.75));

    ControllerGroup* arp = new ControllerGroup(0, std::vector<ParameterController*>
    {
        new ControllerGroup(8, std::vector<ParameterController*>
        {
            new Value(1, 200, 125),
            new Value(1, 233.3, 125),
            new Value(1, 266.6, 125),
            new Value(1, 300, 125),
            new Value(1, 333.3, 125),
            new Value(1, 366.6, 125),
            new Value(1, 400, 125)
        }, ControllerGroup::Order::Random),
        new ControllerGroup(8, std::vector<ParameterController*>
        {
            new Value(1, 200 - 100, 125),
            new Value(1, 233.3 - 100, 125),
            new Value(1, 266.6 - 100, 125),
            new Value(1, 300 - 100, 125),
            new Value(1, 333.3 - 100, 125),
            new Value(1, 366.6 - 100, 125),
            new Value(1, 400 - 100, 125)
        }, ControllerGroup::Order::Random)
    }, ControllerGroup::Order::Forwards);

    arp->start();

    controllerManager->connectParameter(arp, &test->frequency);

    ControllerGroup* pluck = new ControllerGroup(0, std::vector<ParameterController*>
    {
        new Sweep(1, 0, 1, 25),
        new Sweep(1, 1, 0, 100)
    }, ControllerGroup::Order::Forwards);

    pluck->start();

    controllerManager->connectParameter(pluck, &test->volume);

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = Config::CHANNELS;

    unsigned int bufferFrames = Config::BUFFER_LENGTH;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, Config::SAMPLE_RATE, &bufferFrames, &processAudio, (void*)&data))
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

        Config::TIME = time;

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