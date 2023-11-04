#include "../include/organic.h"

struct AudioData
{
    std::vector<AudioSource*> sources;

    Config* config;
};

void Organic::warning(const std::string message)
{
    std::cout << message << "\n";
}

void Organic::error(const std::string message)
{
    std::cout << message << "\n";

    exit(1);
}

void rtAudioError(RtAudioErrorType type, const std::string& message)
{
    Organic::error(message);
}

int processAudio(void* output, void* input, unsigned int frames, double streamTime, RtAudioStreamStatus status, void* userData)
{
    if (status == RTAUDIO_INPUT_OVERFLOW)
    {
        Organic::warning("Stream overflow detected.");
    }

    if (status == RTAUDIO_OUTPUT_UNDERFLOW)
    {
        Organic::warning("Stream underflow detected.");
    }

    double* buffer = (double*)output;

    AudioData* data = (AudioData*)userData;

    std::fill(buffer, buffer + frames * data->config->channels, 0);

    for (AudioSource* source : data->sources)
    {
        source->fillBuffer(buffer, frames);
    }

    for (int i = 0; i < frames * data->config->channels; i += data->config->channels)
    {
        buffer[i] *= data->config->volume;

        if (data->config->channels == 2)
        {
            buffer[i + 1] *= data->config->volume;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        Organic::error("Not enough arguments specified.");
    }

    if (argc > 2)
    {
        Organic::error("Too many arguments specified.");
    }

    CompilerResult compilerResult = Compiler::compile(argv[1]);

    RtAudio audio(RtAudio::Api::UNSPECIFIED, rtAudioError);

    std::vector<unsigned int> ids = audio.getDeviceIds();

    if (ids.size() < 1)
    {
        Organic::error("No audio device detected.");
    }

    Config* config = Config::get();

    AudioData data { compilerResult.sources, config };

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = config->channels;

    unsigned int bufferFrames = config->bufferLength;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, config->sampleRate, &bufferFrames, &processAudio, (void*)&data))
    {
        Organic::error(audio.getErrorText());
    }

    if (audio.startStream())
    {
        if (audio.isStreamOpen())
        {
            audio.closeStream();
        }

        Organic::error(audio.getErrorText());
    }

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();

    while (true)
    {
        config->time = (clock.now() - start).count() / 1000000.0;

        compilerResult.controllerManager->updateControllers();
        compilerResult.eventQueue->performEvents();
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