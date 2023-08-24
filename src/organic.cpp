#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include "../include/RtAudio.h"

#include "../include/constants.h"
#include "../include/audiosource.h"

struct AudioData
{
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
        source->fillBuffer(buffer, frames);
    }

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

    SineAudioSource* test = new SineAudioSource(220, 1);

    data.sources.push_back(test);

    std::vector<Envelope*> envelopes;

    Envelope* one = new Envelope(500, 0, 1, 500, 1);
    Envelope* two = new Envelope(500, 0, 1, 500, 220);

    one->connectValue(&test->volume);
    two->connectValue(&test->frequency);

    envelopes.push_back(one);
    envelopes.push_back(two);

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

    while (true)
    {
        long long time = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - start).count();

        for (Envelope* envelope : envelopes)
        {
            envelope->update(time);
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