#include "../include/organic.h"

Organic::Organic(const Path* path, const ProgramOptions options) :
    options(options)
{
    utils = Utils::get();

    if (options.seed)
    {
        utils->setSeed(options.seed.value());
    }

    Parser::Program* program = (new Parser::ParserCreator())->parse(path);

    program->resolveTypes(new Parser::TypeResolver(path, new Parser::ParserCreator()));

    const Path* bytecodePath = Path::beside(path->stem() + ".obc", path);

    std::ofstream stream(bytecodePath->string(), std::ios::binary);

    if (!stream.is_open())
    {
        throw OrganicFileException("Error creating intermediate file \"" + bytecodePath->string() + "\".");
    }

    program->transform(new Parser::BytecodeTransformer(path, stream));

    stream.close();

    machine = new Machine(bytecodePath);
}

void Organic::start()
{
    if (options.mono)
    {
        utils->channels = 1;
    }

    if (options.exportPath)
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
    RtAudio audio(RtAudio::Api::UNSPECIFIED, std::bind(&Organic::audioError, this, std::placeholders::_2));

    const std::vector<unsigned int>& ids = audio.getDeviceIds();

    if (ids.size() < 1)
    {
        throw OrganicAudioException("No available audio devices detected.");
    }

    RtAudio::StreamParameters parameters;

    parameters.deviceId = audio.getDefaultOutputDevice();
    parameters.nChannels = utils->channels;

    unsigned int bufferFrames = utils->bufferLength;

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, utils->sampleRate, &bufferFrames, std::bind(&Organic::processAudio, this, std::placeholders::_1, std::placeholders::_3), nullptr))
    {
        throw OrganicAudioException(audio.getErrorText());
    }

    if (audio.startStream())
    {
        if (audio.isStreamOpen())
        {
            audio.closeStream();
        }

        throw OrganicAudioException(audio.getErrorText());
    }

    machine->run();

    while (!callbackActive) {}

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<std::chrono::high_resolution_clock> start = clock.now();

    while (!options.time || utils->time < options.time.value())
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
    const unsigned int steps = (options.time.value() / 1000) * utils->sampleRate;

    SndfileHandle* file = new SndfileHandle(options.exportPath.value()->string(), SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_32, utils->channels, utils->sampleRate);

    double* samples = (double*)malloc(sizeof(double) * steps * utils->channels);

    machine->run();

    for (unsigned int i = 0; i < steps; i += utils->bufferLength)
    {
        utils->time = i * 1000.0 / utils->sampleRate;

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
        throw OrganicFileException("Could not write output file: " + std::string(file->strError()));
    }
}

int Organic::processAudio(void* output, unsigned int frames)
{
    callbackActive = true;

    machine->processAudioSources((double*)output, frames);

    return 0;
}

void Organic::audioError(const std::string message) const
{
    throw OrganicAudioException(message);
}
