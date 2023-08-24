#include <iostream>
#include <vector>
#include <string>

#include "../include/RtAudio.h"

#include "../include/audiosource.h"

#define SAMPLE_RATE 44100

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

    for (int i = 0; i < frames * 2; i++)
    {
        buffer[i] = 0;
    }

    for (AudioSource* source : data->sources)
    {
        source->fillBuffer(buffer, frames, data->phase);
    }

    data->phase = (data->phase + frames) % SAMPLE_RATE;

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
    parameters.nChannels = 2;
    parameters.firstChannel = 0;

    unsigned int sampleRate = SAMPLE_RATE;
    unsigned int bufferFrames = 256;

    AudioData data;

    data.sources.push_back(new TestAudioSource(sampleRate));

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, sampleRate, &bufferFrames, &processAudio, (void*)&data))
    {
        error(audio.getErrorText());
    }

    if (audio.startStream())
    {
        errorAndDestroy(audio.getErrorText(), audio);
    }

    char input;

    std::cin.get(input);

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