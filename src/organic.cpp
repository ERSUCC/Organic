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
        source->fillBuffer(buffer, frames);
    }

    for (int i = 0; i < frames * data->utils->channels; i += data->utils->channels)
    {
        buffer[i] *= data->utils->volume;

        if (data->utils->channels == 2)
        {
            buffer[i + 1] *= data->utils->volume;
        }
    }

    return 0;
}

Organic::Organic(std::vector<AudioSource*> audioSources, EventQueue* eventQueue, InterpreterOptions options) :
    audioSources(audioSources), eventQueue(eventQueue), options(options)
{
    utils = Utils::get();
}

void Organic::init(const std::string program, const std::vector<const std::string>& flags)
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

    if (options.test)
    {
        startTest();
    }

    else
    {
        startPlayback();
    }
}

void Organic::startTest()
{
    std::ostringstream result;

    for (double frame = 0; frame <= options.time * utils->sampleRate; frame++)
    {
        utils->time = frame / utils->sampleRate;

        double* buffer = (double*)calloc(utils->channels, sizeof(double));

        for (AudioSource* source : audioSources)
        {
            source->fillBuffer(buffer, 1);
        }

        char value[21];

        snprintf(value, 21, "%0.20f", buffer[0] * utils->volume);

        result << value;

        if (utils->channels == 2)
        {
            snprintf(value, 21, "%0.20f", buffer[1] * utils->volume);

            result << value;
        }
    }

    std::ifstream input(options.testFile);

    if (!input.is_open())
    {
        Utils::error("Could not open testing file.");
    }

    std::stringstream compare;

    compare << input.rdbuf();

    input.close();

    if (compare.str() != result.str())
    {
        Utils::error("Failed test.");
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

    while (true)
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
