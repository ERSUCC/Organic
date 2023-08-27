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

    FiniteSequence* seq = new FiniteSequence(std::vector<double> {
        130.81, 146.83, 164.81, 185, 207.65, 233.08, 261.63
    }, FiniteSequence::Order::PingPong);
    FiniteSequence* seq2 = new FiniteSequence(std::vector<double> {
        130.81, 146.83, 164.81, 185, 207.65, 233.08, 261.63
    }, FiniteSequence::Order::Forwards);
    Sweep* pluck = new Sweep(1, 0, 0);
    LFO* length = new LFO(200, 50, 9600);

    length->connectParameter(&pluck->length);

    parameterControllers.push_back(seq);
    parameterControllers.push_back(seq2);
    parameterControllers.push_back(pluck);
    parameterControllers.push_back(length);

    AudioData data;

    Square* square = new Square(1, 0, 0);

    seq->connectParameter(&square->frequency);
    pluck->connectParameter(&square->volume);

    seq->start(0);
    pluck->start(0);
    length->start(0);

    Sine* bass = new Sine(1, 0, 0);

    seq2->connectParameter(&bass->frequency);

    seq2->start(0);

    data.sources.push_back(square);
    data.sources.push_back(bass);

    auto cmp = [](Event* left, Event* right)
    {
        return left->next > right->next;
    };

    std::priority_queue<Event*, std::vector<Event*>, decltype(cmp)> eventQueue(cmp);

    eventQueue.push(new IntervalEvent([=](double time)
    {
        seq->next(time);
        pluck->start(time);
    }, 100, 100));

    eventQueue.push(new IntervalEvent([=](double time)
    {
        seq2->next(time);
    }, 300, 300));

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