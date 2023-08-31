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

    EffectManager* effectManager;
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

    for (AudioSource* source : data->sources)
    {
        source->prepareForEffects(frames);
    }

    data->effectManager->applyEffects(streamTime * 1000);

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

    LFO* volume1 = new LFO(0, 1.5, 20000);
    LFO* volume2 = new LFO(0, 1, 20000);
    LFO* volume3 = new LFO(0, 1, 20000);
    LFO* volume4 = new LFO(0, 0.75, 20000);

    volume1->start(0);
    volume2->start(0);
    volume3->start(0);
    volume4->start(0);

    controllerManager->addController(volume1);
    controllerManager->addController(volume2);
    controllerManager->addController(volume3);
    controllerManager->addController(volume4);

    FiniteSequence* seq1 = new FiniteSequence(std::vector<double>
    {
        110, 82.41, 98, 82.41
    }, FiniteSequence::Order::Forwards);
    FiniteSequence* seq2 = new FiniteSequence(std::vector<double>
    {
        164.81, 164.81, 196, 220
    }, FiniteSequence::Order::Forwards);
    FiniteSequence* seq3 = new FiniteSequence(std::vector<double>
    {
        220, 220, 246.94, 246.94
    }, FiniteSequence::Order::Forwards);
    FiniteSequence* seq4 = new FiniteSequence(std::vector<double>
    {
        261.63, 261.63, 329.63, 293.66
    }, FiniteSequence::Order::Forwards);

    seq1->start(0);
    seq2->start(0);
    seq3->start(0);
    seq4->start(0);

    controllerManager->addController(seq1);
    controllerManager->addController(seq2);
    controllerManager->addController(seq3);
    controllerManager->addController(seq4);

    EffectManager* effectManager = new EffectManager();

    LowPassFilter* filter = new LowPassFilter(1000);

    effectManager->addEffect(filter);

    AudioData data;

    data.effectManager = effectManager;

    Saw* saw1 = new Saw(0, 0, 0);
    Saw* saw2 = new Saw(0, 0, 0);
    Saw* saw3 = new Saw(0, 0, 0);
    Saw* saw4 = new Saw(0, 0, 0);

    controllerManager->connectParameter(volume1, &saw1->volume);
    controllerManager->connectParameter(volume2, &saw2->volume);
    controllerManager->connectParameter(volume3, &saw3->volume);
    controllerManager->connectParameter(volume4, &saw4->volume);;

    controllerManager->connectParameter(seq1, &saw1->frequency);
    controllerManager->connectParameter(seq2, &saw2->frequency);
    controllerManager->connectParameter(seq3, &saw3->frequency);
    controllerManager->connectParameter(seq4, &saw4->frequency);

    //data.sources.push_back(saw1);
    //data.sources.push_back(saw2);
    //data.sources.push_back(saw3);
    //data.sources.push_back(saw4);

    Saw* test = new Saw(1, 0, 110);

    effectManager->connectAudioSource(filter, test);

    data.sources.push_back(test);

    EventQueue* eventQueue = new EventQueue();

    eventQueue->addEvent(new IntervalEvent([=](double time)
    {
        seq1->next(time);
        seq2->next(time);
        seq3->next(time);
        seq4->next(time);
    }, 0, 20000, 20000));

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

        audio.setStreamTime(time / 1000);

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