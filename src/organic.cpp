#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <queue>

#include "../include/RtAudio.h"

#include "../include/config.h"
#include "../include/audiosource.h"
#include "../include/parameter.h"
#include "../include/event.h"

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
    if (status)
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
        source->fillBuffer(buffer, frames);
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

    std::vector<ParameterController*> parameterControllers;

    LFO* pan = new LFO(-1, 1, 50);
    Sweep* volume = new Sweep(1, 0, 2000);
    Sweep* rate = new Sweep(-1, 0, 2000);
    Sweep* rate2 = new Sweep(1, 0, 2000);

    pan->start(0);

    parameterControllers.push_back(pan);
    parameterControllers.push_back(volume);
    parameterControllers.push_back(rate);
    parameterControllers.push_back(rate2);

    AudioData data;

    Square* square = new Square(1, 0, 220);

    pan->connectParameter(&square->pan);
    volume->connectParameter(&square->volume);
    rate->connectParameter(&pan->floor);
    rate2->connectParameter(&pan->ceiling);

    data.sources.push_back(square);

    std::priority_queue<Event*, std::vector<Event*>, std::greater<Event*>> eventQueue;

    eventQueue.push(new IntervalEvent([volume, rate, rate2](double time)
    {
        volume->start(time);
        rate->start(time);
        rate2->start(time);
    }, 0, 4000));

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

    while (true)
    {
        double time = (clock.now() - start).count() / 1000000.0;

        while (!eventQueue.empty() && eventQueue.top()->ready(time))
        {
            Event* event = eventQueue.top();

            eventQueue.pop();

            event->perform(time);

            if (!event->discard)
            {
                eventQueue.push(event);
            }
        }

        for (ParameterController* parameterController : parameterControllers)
        {
            parameterController->update(time);
        }
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