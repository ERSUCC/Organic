#include "../include/organic.h"

void rtAudioError(RtAudioErrorType type, const std::string& message)
{
    Utils::error(message);
}

int processAudio(void* output, void* input, unsigned int frames, double streamTime, RtAudioStreamStatus status, void* userData)
{
    AudioData* data = (AudioData*)userData;

    data->callbackActive = true;

    data->machine->processAudioSources((double*)output, frames);

    return 0;
}

Organic::Organic(const std::string program, const std::vector<std::string>& flags)
{
    utils = Utils::get();

    options = (new Interpreter(program, flags))->interpret();

    machine = new Machine(options.bytecodePath);
}

void Organic::start()
{
    if (options.setMono)
    {
        utils->channels = 1;
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

    const std::vector<unsigned int>& ids = audio.getDeviceIds();

    if (ids.size() < 1)
    {
        Utils::error("No audio device detected.");
    }

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = utils->channels;

    AudioData audioData { utils, machine };

    unsigned int bufferFrames = utils->bufferLength;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, utils->sampleRate, &bufferFrames, &processAudio, (void*)&audioData))
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

    std::vector<ValueObject*> inputs; // could be used for command line inputs or something

    machine->execute(4, inputs, 0);

    while (!audioData.callbackActive) {}

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();

    while (!options.setTime || utils->time < options.time)
    {
        utils->time = (clock.now() - start).count() / 1000000.0;

        machine->performEvents();
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
    const unsigned int steps = (options.time / 1000) * utils->sampleRate;

    AudioFile<double> file;

    file.setNumChannels(utils->channels);
    file.setNumSamplesPerChannel(steps);

    double* buffer = (double*)malloc(sizeof(double) * utils->bufferLength * utils->channels);

    std::vector<ValueObject*> inputs; // could be used for command line inputs or something

    machine->execute(4, inputs, 0);

    for (unsigned int i = 0; i < steps; i += utils->bufferLength)
    {
        utils->time = i * 1000.0 / utils->sampleRate;

        machine->performEvents();
        machine->processAudioSources(buffer, utils->bufferLength);

        for (int j = 0; j < utils->bufferLength && i + j < steps; j++)
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
