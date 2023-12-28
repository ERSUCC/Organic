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

Organic::Organic(std::vector<AudioSource*> audioSources, EventQueue* eventQueue, InterpreterOptions options) :
    audioSources(audioSources), eventQueue(eventQueue), options(options)
{
    utils = Utils::get();
}

void Organic::init(const std::string program, const std::vector<std::string>& flags)
{
    InterpreterResult result = Interpreter::interpret(program, flags);

    Organic* instance = new Organic(result.sources, result.eventQueue, result.options);

    instance->start();
}

void Organic::start()
{
    for (AudioSource* audioSource : audioSources)
    {
        audioSource->start();
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

    AudioFile<double>::AudioBuffer fileBuffer;

    fileBuffer.resize(utils->bufferLength);

    fileBuffer[0].resize(steps);

    if (utils->channels == 2)
    {
        fileBuffer[1].resize(steps);
    }

    double* buffer = (double*)malloc(sizeof(double) * utils->bufferLength * utils->channels);

    for (int i = 0; i < steps; i += utils->bufferLength)
    {
        std::fill(buffer, buffer + utils->bufferLength * utils->channels, 0);

        for (AudioSource* source : audioSources)
        {
            source->fillBuffer(buffer, utils->bufferLength, utils->volume);
        }

        for (int j = 0; j < utils->bufferLength; j++)
        {
            fileBuffer[0][i + j] = buffer[j * utils->channels];

            if (utils->channels == 2)
            {
                fileBuffer[1][i + j] = buffer[j * 2 + 1];
            }
        }
    }

    AudioFile<double> file;

    file.setAudioBuffer(fileBuffer);
    file.save(options.exportPath);
}
