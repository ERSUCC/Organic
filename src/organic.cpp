#include "../include/organic.h"

Organic::Organic(const std::filesystem::path& path, const std::vector<std::string>& flags)
{
    utils = Utils::get();

    options = (new FlagParser(flags))->getOptions();

    const std::filesystem::path& bytecodePath = std::filesystem::weakly_canonical(path.parent_path() / (path.stem().string() + ".obc"));

    std::ofstream stream(bytecodePath, std::ios::binary);

    if (!stream.is_open())
    {
        Utils::fileError("Error creating intermediate file \"" + bytecodePath.string() + "\".");
    }

    (new Parser::BytecodeTransformer(path, stream))->visit((new Parser::Parser(path))->parse());

    stream.close();

    machine = new Machine(bytecodePath);
}

void Organic::start()
{
    if (options.mono)
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
    RtAudio audio(RtAudio::Api::UNSPECIFIED, std::bind(&Utils::audioError, std::placeholders::_2));

    const std::vector<unsigned int>& ids = audio.getDeviceIds();

    if (ids.size() < 1)
    {
        Utils::audioError("No available audio devices detected.");
    }

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = utils->channels;

    unsigned int bufferFrames = utils->bufferLength;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, utils->sampleRate, &bufferFrames, std::bind(&Organic::processAudio, this, std::placeholders::_1, std::placeholders::_3), nullptr))
    {
        Utils::audioError(audio.getErrorText());
    }

    if (audio.startStream())
    {
        if (audio.isStreamOpen())
        {
            audio.closeStream();
        }

        Utils::audioError(audio.getErrorText());
    }

    machine->run();

    while (!callbackActive) {}

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();

    while (!options.setTime || utils->time < options.time)
    {
        utils->time = (clock.now() - start).count() / 1000000.0;
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

    SndfileHandle* file = new SndfileHandle(options.exportPath.string(), SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_32, utils->channels, utils->sampleRate);

    double* samples = (double*)malloc(sizeof(double) * steps * utils->channels);

    machine->run();

    for (unsigned int i = 0; i < steps; i += utils->bufferLength)
    {
        utils->time = i * 1000.0 / utils->sampleRate;

        machine->updateEvents();

        if (i + utils->bufferLength < steps)
        {
            machine->processAudioSources(samples + i * utils->channels, utils->bufferLength);
        }

        else
        {
            machine->processAudioSources(samples + i * utils->channels, steps - i);
        }
    }

    if (file->write(samples, steps * utils->channels) != steps * utils->channels)
    {
        Utils::fileError("Could not write output file: " + std::string(file->strError()));
    }
}

int Organic::processAudio(void* output, unsigned int frames)
{
    callbackActive = true;

    machine->updateEvents();
    machine->processAudioSources((double*)output, frames);

    return 0;
}
