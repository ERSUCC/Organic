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
        error("Error: Stream overflow detected.");
    }

    if (status == RTAUDIO_OUTPUT_UNDERFLOW)
    {
        error("Error: Stream underflow detected.");
    }

    double* buffer = (double*)output;

    AudioData* data = (AudioData*)userData;

    for (int i = 0; i < frames * Config::CHANNELS; i++)
    {
        buffer[i] = 0;
    }

    for (AudioSource* source : data->sources)
    {
        source->fillBuffer(buffer, frames, streamTime);
    }

    for (int i = 0; i < frames * Config::CHANNELS; i++)
    {
        buffer[i] *= Config::MASTER_VOLUME;
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

    Saw* boop = new Saw(1, 0, 0);

    Sweep* pluck = new Sweep(1, 0, 0);
    Sweep* length = new Sweep(300, 750, 6000);
    FiniteSequence* pitch = new FiniteSequence(std::vector<double>
    {
        110, 220, 246.94, 277.18, 311.13, 329.63, 369.99, 415.3, 440
    }, FiniteSequence::Order::Random);

    controllerManager->addController(pluck);
    controllerManager->addController(length);
    controllerManager->addController(pitch);

    controllerManager->connectParameter(pluck, &boop->volume);
    controllerManager->connectParameter(length, &pluck->length);
    controllerManager->connectParameter(pitch, &boop->frequency);

    Delay* delay = new Delay(375, 0.85);
    LowPassFilter* low = new LowPassFilter(0);

    boop->addEffect(delay);
    boop->addEffect(low);

    Sweep* filterSweep = new Sweep(0, 10000, 15000);

    controllerManager->addController(filterSweep);

    controllerManager->connectParameter(filterSweep, &low->cutoff);

    data.sources.push_back(boop);

    Triangle* bell = new Triangle(0, 0, 0);

    Sweep* pluck2 = new Sweep(1, 0, 150);
    FiniteSequence* pitch2 = new FiniteSequence(std::vector<double>
    {
        880, 1244.51, 1318.51
    }, FiniteSequence::Order::Random);

    controllerManager->addController(pluck2);
    controllerManager->addController(pitch2);

    controllerManager->connectParameter(pluck2, &bell->volume);
    controllerManager->connectParameter(pitch2, &bell->frequency);

    Delay* delay2 = new Delay(375, 0.75);

    bell->addEffect(delay2);

    data.sources.push_back(bell);

    EventQueue* eventQueue = new EventQueue();

    eventQueue->addEvent(new IntervalEvent([=](double time, double target)
    {
        length->start(time);
        filterSweep->start(time);

        eventQueue->addEvent(new RepeatedEvent([=](double time, double target)
        {
            eventQueue->addEvent(new RandomRepeatedEvent([=](double time, double target)
            {
                boop->phase = 0;

                pluck->start(time);
                pitch->next(time);
            }, target, 0, 125, 1000, 125, 4));
        }, target, 0, 2000, 4));

        eventQueue->addEvent(new RandomRepeatedEvent([=](double time, double target)
        {
            pluck2->start(time);
            pitch2->next(time);
        }, target, 2500, 125, 500, 125, 15));
    }, 0, 0, 15000));

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

        audio.setStreamTime(time);

        eventQueue->performEvents(time);
        controllerManager->updateControllers(time);
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