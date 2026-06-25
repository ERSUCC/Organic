#include "../include/organic.h"

Organic::Organic(const Path* path, const ProgramOptions options) :
    options(options)
{
    utils = Utils::get();

    utils->channels = options.channels.value_or(2);
    utils->sampleRate = options.sampleRate.value_or(44100);
    utils->bufferLength = options.bufferLength.value_or(128);
    utils->setSeed(options.seed);

    if (options.info.value_or(false))
    {
        Utils::printInfo();
    }

    const Parser::Program* program = Parser::Parser::parseSource(path);

    program->resolveTypes(new Parser::TypeResolver());

    this->program = program->transform(new TokenTransformer(path))->getLeafAs<Engine::Program>();
}

void Organic::start()
{
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

    if (audio.openStream(&parameters, nullptr, RTAUDIO_FLOAT64, utils->sampleRate, &utils->bufferLength, std::bind(&Organic::processAudio, this, std::placeholders::_1, std::placeholders::_3), nullptr))
    {
        throw OrganicAudioException(audio.getErrorText());
    }

    program->start(0);

    if (options.fastForward)
    {
        const size_t frames = utils->sampleRate * options.fastForward.value() / 1000;

        double* buffer = (double*)malloc(sizeof(double) * utils->channels);

        for (size_t i = 0; i < frames; i++)
        {
            utils->time = i * 1000.0 / utils->sampleRate;

            program->processAudioSources(buffer);
        }

        free(buffer);
    }

    if (audio.startStream())
    {
        if (audio.isStreamOpen())
        {
            audio.closeStream();
        }

        throw OrganicAudioException(audio.getErrorText());
    }

    if (options.time.has_value())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds((long long)options.time.value()));
    }

    else
    {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(LLONG_MAX));
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
}

void Organic::startExport()
{
    const size_t steps = (options.time.value() / 1000) * utils->sampleRate;

    SndfileHandle* file = new SndfileHandle(options.exportPath.value()->string(), SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_24, utils->channels, utils->sampleRate);

    double* samples = (double*)malloc(sizeof(double) * steps * utils->channels);

    program->start(0);

    for (size_t i = 0; i < steps; i++)
    {
        utils->time = i * 1000.0 / utils->sampleRate;

        program->processAudioSources(samples + i * utils->channels);
    }

    if (file->write(samples, steps * utils->channels) != steps * utils->channels)
    {
        throw OrganicFileException("Could not write output file: " + std::string(file->strError()));
    }

    free(samples);
}

int Organic::processAudio(void* output, unsigned int frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        program->processAudioSources((double*)output + i * utils->channels);

        utils->time += 1000.0 / utils->sampleRate;
    }

    return 0;
}

void Organic::audioError(const std::string message) const
{
    throw OrganicAudioException(message);
}
