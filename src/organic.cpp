#include "../include/organic.h"

void rtAudioError(RtAudioErrorType type, const std::string& message)
{
    Utils::error(message);
}

int processAudio(void* output, void* input, unsigned int frames, double streamTime, RtAudioStreamStatus status, void* userData)
{
    AudioData* data = (AudioData*)userData;

    double* buffer = (double*)output;

    std::fill(buffer, buffer + frames * data->utils->channels, 0);

    for (AudioSource* source : data->audioSources)
    {
        source->fillBuffer(buffer, frames, data->utils->volume);
    }

    return 0;
}

Organic::Organic(const std::string program, const std::vector<std::string> flags)
{
    utils = Utils::get();

    Interpreter* interpreter = new Interpreter(program, flags);

    interpreter->interpret();

    audioSources = interpreter->sources;
    eventQueue = interpreter->eventQueue;
    options = interpreter->options;
}

void Organic::start()
{
    if (options.setMono)
    {
        utils->channels = 1;
    }

    for (AudioSource* audioSource : audioSources)
    {
        audioSource->start(0);
    }

    if (options.setExport)
    {
        startExport();
    }

    else
    {
        startPlayback();
    }
}

void Organic::startPlayback()
{
    RtAudio audio(RtAudio::Api::UNSPECIFIED, rtAudioError);

    std::vector<unsigned int> ids = audio.getDeviceIds();

    if (ids.size() < 1)
    {
        Utils::error("No audio device detected.");
    }

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = utils->channels;

    unsigned int bufferFrames = utils->bufferLength;

    AudioData data { audioSources, utils };

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, utils->sampleRate, &bufferFrames, &processAudio, (void*)&data))
    {
        Utils::error(audio.getErrorText());
    }

    if (audio.startStream())
    {
        if (audio.isStreamOpen())
        {
            audio.closeStream();
        }

        Utils::error(audio.getErrorText());
    }

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();

    while (!options.setTime || utils->time < options.time)
    {
        utils->time = (clock.now() - start).count() / 1000000.0;

        eventQueue->performEvents();
    }

    if (audio.isStreamRunning())
    {
        audio.stopStream();
    }

    if (audio.isStreamOpen())
    {
        audio.closeStream();
    }
}

void Organic::startExport()
{
    int steps = (options.time / 1000) * utils->sampleRate;

    AudioFile<double> file;

    file.setNumChannels(utils->channels);
    file.setNumSamplesPerChannel(steps);

    double* buffer = (double*)malloc(sizeof(double) * utils->bufferLength * utils->channels);

    for (int i = 0; i < steps; i += utils->bufferLength)
    {
        utils->time = i * 1000.0 / utils->sampleRate;

        std::fill(buffer, buffer + utils->bufferLength * utils->channels, 0);

        for (AudioSource* source : audioSources)
        {
            source->fillBuffer(buffer, utils->bufferLength, utils->volume);
        }

        for (int j = 0; j < utils->bufferLength; j++)
        {
            file.samples[0][i + j] = buffer[j * utils->channels];

            if (utils->channels == 2)
            {
                file.samples[1][i + j] = buffer[j * 2 + 1];
            }
        }
    }

    file.save(options.exportPath);
}
