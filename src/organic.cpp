#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include "../include/RtAudio.h"

#include "../include/constants.h"
#include "../include/audiosource.h"

struct AudioData
{
    unsigned int phase = 0;

    std::vector<AudioSource*> sources;
};

void error(const std::string& message)
{
    std::cout << message << "\n";

    exit(0);
}

void errorAndDestroy(const std::string& message, RtAudio audio)
{
    if (audio.isStreamOpen())
    {
        audio.closeStream();
    }

    error(message);
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
        source->fillBuffer(buffer, frames, data->phase);
    }

    data->phase = (data->phase + frames) % Constants::SAMPLE_RATE;

    return 0;
}

int main(int argc, char** argv)
{
    RtAudio audio(RtAudio::Api::MACOSX_CORE, rtAudioError);

    std::vector<unsigned int> ids = audio.getDeviceIds();

    if (ids.size() < 1)
    {
        error("Error: No audio device detected.");
    }

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = Constants::CHANNELS;
    parameters.firstChannel = 0;

    unsigned int bufferFrames = Constants::BUFFER_LENGTH;

    AudioData data;

    data.sources.push_back(new SineAudioSource(220, 1, 50, 300, 0, 0));

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, Constants::SAMPLE_RATE, &bufferFrames, &processAudio, (void*)&data))
    {
        error(audio.getErrorText());
    }

    if (audio.startStream())
    {
        errorAndDestroy(audio.getErrorText(), audio);
    }

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();

    bool retrigger;

    while (true)
    {
        long long time = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - start).count();

        if (time % 500 == 0 && retrigger)
        {
            for (AudioSource* source : data.sources)
            {
                source->trigger(time);
            }

            retrigger = false;
        }

        else if (time % 500 > 0)
        {
            retrigger = true;
        }

        for (AudioSource* source : data.sources)
        {
            source->update(time);
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