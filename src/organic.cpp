#include "organic.h"

Organic::Organic(const ProgramOptions& options) :
    options(options)
{
    utils = Utils::get();

    utils->channels = options.channels.value_or(2);
    utils->sampleRate = options.sampleRate.value_or(44100);
    utils->bufferLength = options.bufferLength.value_or(128);
    utils->timeStep = 1000.0 / utils->sampleRate;

    utils->setSeed(options.seed);

    if (options.info.value_or(false))
    {
        Utils::printInfo();
    }

    const Path& path = options.programPath.value();

    const FileProvider* source = FileProvider::create(path);

    if (!source)
    {
        throw OrganicFileException("Could not read \"" + path.string() + "\".");
    }

    const Parser::Program* program = Parser::Parser::parseSource(source);

    Parser::TypeResolver* resolver = new Parser::TypeResolver();

    try
    {
        program->resolveTypes(resolver);
    }

    catch (const OrganicException& ex)
    {
        delete resolver;

        throw;
    }

    delete resolver;

    TokenTransformer* transformer = new TokenTransformer(path);

    this->program = program->transform(transformer);

    delete transformer;
    delete program;
    delete source;
}

Organic::~Organic()
{
    delete program;
    delete utils;

    Engine::Defaults::deinit();
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

void Organic::stop()
{
    std::unique_lock unique(lock);

    running = false;

    signal.notify_one();
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
            utils->time = i * utils->timeStep;

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

    handleInterrupt(this);

    std::unique_lock unique(lock);

    if (options.time.has_value())
    {
        std::cv_status status;

        do
        {
            status = signal.wait_for(unique, std::chrono::milliseconds((long long)options.time.value()));
        } while (running && status != std::cv_status::timeout);
    }

    else
    {
        do
        {
            signal.wait(unique);
        } while (running);
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
    const size_t frames = steps * utils->channels;

    SndfileHandle* file = new SndfileHandle(options.exportPath.value().string(), SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_24, utils->channels, utils->sampleRate);

    double* samples = (double*)malloc(sizeof(double) * frames);

    double max = 0;

    program->start(0);

    for (size_t i = 0; i < steps; i++)
    {
        utils->time = i * utils->timeStep;

        program->processAudioSources(samples + i * utils->channels);

        for (size_t j = 0; j < utils->channels; j++)
        {
            const double mag = fabs(samples[i * utils->channels + j]);

            if (mag > max)
            {
                max = mag;
            }
        }
    }

    if (max > 1)
    {
        for (size_t i = 0; i < frames; i++)
        {
            samples[i] /= max;
        }
    }

    const sf_count_t written = file->write(samples, frames);

    free(samples);

    if (written != frames)
    {
        const std::string error = file->strError();

        delete file;

        throw OrganicFileException("Could not write output file: " + error);
    }

    delete file;
}

int Organic::processAudio(void* output, const unsigned int frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        program->processAudioSources((double*)output + i * utils->channels);

        utils->time += utils->timeStep;
    }

    return 0;
}

void Organic::audioError(const std::string& message) const
{
    throw OrganicAudioException(message);
}

#ifdef _WIN32

#include <Windows.h>

Organic* instance = nullptr;

BOOL WINAPI handler(DWORD control)
{
    if (control == CTRL_C_EVENT && instance)
    {
        instance->stop();

        return TRUE;
    }

    return FALSE;
}

void Organic::handleInterrupt(Organic* organic)
{
    instance = organic;

    SetConsoleCtrlHandler(&handler, true);
}

#else

#include <signal.h>

Organic* instance = nullptr;

void handler(int sig)
{
    if (instance)
    {
        instance->stop();
    }
}

void Organic::handleInterrupt(Organic* organic)
{
    instance = organic;

    struct sigaction action = { 0 };

    action.sa_handler = &handler;

    sigaction(SIGINT, &action, nullptr);
}

#endif
