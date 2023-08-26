#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include "../include/RtAudio.h"

#include "../include/constants.h"
#include "../include/audiosource.h"
#include "../include/parameter.h"

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

    for (int i = 0; i < frames * Constants::CHANNELS; i++)
    {
        buffer[i] = 0;
    }

    for (AudioSource* source : data->sources)
    {
        source->fillBuffer(buffer, frames);
    }

    for (int i = 0; i < frames * Constants::CHANNELS; i++)
    {
        buffer[i] *= Constants::MASTER_VOLUME;
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

    Sweep* pitch = new Sweep(196, 261.63, 10000);
    Sweep* pitch2 = new Sweep(196, 130.81, 10000);

    parameterControllers.push_back(pitch);
    parameterControllers.push_back(pitch2);

    AudioData data;

    SquareAudioSource* square = new SquareAudioSource(1, 0, 0);
    SquareAudioSource* square2 = new SquareAudioSource(1, 0, 0);

    pitch->connectParameter(&square->frequency);
    pitch2->connectParameter(&square2->frequency);

    data.sources.push_back(square);
    data.sources.push_back(square2);

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = Constants::CHANNELS;

    unsigned int bufferFrames = Constants::BUFFER_LENGTH;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, Constants::SAMPLE_RATE, &bufferFrames, &processAudio, (void*)&data))
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

    pitch->start(0);
    pitch2->start(0);

    while (true)
    {
        double time = (clock.now() - start).count() / 1000000.0;

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