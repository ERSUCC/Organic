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

    ControllerManager* controllerManager = new ControllerManager();

    FiniteSequence* seq = new FiniteSequence(std::vector<double> {
        130.81, 146.83, 164.81, 185, 207.65, 233.08, 261.63
    }, FiniteSequence::Order::PingPong);
    FiniteSequence* seq2 = new FiniteSequence(std::vector<double> {
        130.81, 146.83, 164.81, 185, 207.65, 233.08, 261.63
    }, FiniteSequence::Order::Random);
    Sweep* pluck = new Sweep(1, 0, 0);
    LFO* length = new LFO(200, 50, 9600);

    controllerManager->addController(seq);
    controllerManager->addController(seq2);
    controllerManager->addController(pluck);
    controllerManager->addController(length);

    controllerManager->connectParameter(length, &pluck->length);

    length->start(0);

    AudioData data;

    Square* square = new Square(1, 0, 0);

    controllerManager->connectParameter(seq, &square->frequency);
    controllerManager->connectParameter(pluck, &square->volume);

    seq->start(0);
    pluck->start(0);

    Sine* bass = new Sine(1, 0, 0);

    controllerManager->connectParameter(seq2, &bass->frequency);

    seq2->start(0);

    data.sources.push_back(square);
    data.sources.push_back(bass);

    EventQueue* eventQueue = new EventQueue();

    eventQueue->addEvent(new IntervalEvent([=](double time)
    {
        seq->next(time);
        pluck->start(time);
    }, 0, 100, 100));

    eventQueue->addEvent(new IntervalEvent([=](double time)
    {
        seq2->next(time);
    }, 0, 600, 600));

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