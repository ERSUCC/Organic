#include "../include/audiosource.h"

SingleAudioSource::SingleAudioSource(ValueObject* volume, ValueObject* pan, ValueObject* effects) :
    volume(volume), pan(pan), effects(effects)
{
    effectBuffer = (double*)malloc(sizeof(double) * utils->bufferLength * utils->channels);
}

SingleAudioSource::~SingleAudioSource()
{
    free(effectBuffer);
}

void SingleAudioSource::fillBuffer(double* buffer, const unsigned int bufferLength)
{
    prepareForEffects(bufferLength);

    for (ValueObject* effect : effects->getList()->objects)
    {
        dynamic_cast<Effect*>(effect->expandVariable())->apply(effectBuffer, bufferLength);
    }

    for (unsigned int i = 0; i < bufferLength * utils->channels; i++)
    {
        buffer[i] += effectBuffer[i];
    }
}

Oscillator::Oscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    SingleAudioSource(volume, pan, effects), frequency(frequency) {}

void Oscillator::init()
{
    volume->start(startTime);
    pan->start(startTime);
    frequency->start(startTime);

    for (ValueObject* effect : effects->getList()->objects)
    {
        effect->start(startTime);
    }
}

void Oscillator::prepareForEffects(const unsigned int bufferLength)
{
    phaseDelta = utils->twoPi * frequency->getValue() / utils->sampleRate;

    const double volumeValue = volume->getValue();
    const double panValue = pan->getValue();

    for (int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        const double value = volumeValue * getValue();

        if (utils->channels == 1)
        {
            effectBuffer[i] = value;
        }

        else
        {
            effectBuffer[i] = value * (1 - panValue) / 2;
            effectBuffer[i + 1] = value * (panValue + 1) / 2;
        }

        phase += phaseDelta;
    }

    if (phase > utils->twoPi)
    {
        phase -= utils->twoPi;
    }
}

Sine::Sine(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Sine::getValue()
{
    return sin(phase);
}

Square::Square(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Square::getValue()
{
    if (sin(phase) > 0)
    {
        return -1;
    }

    return 1;
}

Saw::Saw(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Saw::getValue()
{
    return fmod(phase, utils->twoPi) / utils->pi - 1;
}

Triangle::Triangle(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Triangle::getValue()
{
    return 2 * fabs(fmod(phase, utils->twoPi) - utils->pi) / utils->pi - 1;
}

Noise::Noise(ValueObject* volume, ValueObject* pan, ValueObject* effects) :
    SingleAudioSource(volume, pan, effects) {}

void Noise::prepareForEffects(const unsigned int bufferLength)
{
    const double volumeValue = volume->getValue();
    const double panValue = pan->getValue();

    for (int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        const double value = volumeValue * udist(utils->rng);

        if (utils->channels == 1)
        {
            effectBuffer[i] = value;
        }

        else
        {
            effectBuffer[i] = value * (1 - panValue) / 2;
            effectBuffer[i + 1] = value * (panValue + 1) / 2;
        }
    }
}

Sample::Sample(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource) :
    SingleAudioSource(volume, pan, effects), resource(resource) {}

void Sample::init()
{
    volume->start(startTime);
    pan->start(startTime);
    resource->start(startTime);

    for (ValueObject* effect : effects->getList()->objects)
    {
        effect->start(startTime);
    }

    index = 0;
}

void Sample::prepareForEffects(const unsigned int bufferLength)
{
    const double volumeValue = volume->getValue();
    const double panValue = pan->getValue();

    // improve resource/sample acquisition
    for (unsigned int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        const double value = volumeValue * resource->getResource()->samples[(unsigned int)floor(index)];

        if (utils->channels == 1)
        {
            effectBuffer[i] = value;
        }

        else
        {
            effectBuffer[i] = value * (1 - panValue) / 2;
            effectBuffer[i + 1] = value * (panValue + 1) / 2;
        }

        index = fmod(index + (double)resource->getResource()->sampleRate / utils->sampleRate, resource->getResource()->length);
    }
}

Blend::Blend(ValueObject* audioSources, ValueObject* position) :
    audioSources(audioSources), position(position)
{
    buffer1 = (double*)malloc(sizeof(double) * utils->bufferLength * utils->channels);
    buffer2 = (double*)malloc(sizeof(double) * utils->bufferLength * utils->channels);
}

Blend::~Blend()
{
    free(buffer1);
    free(buffer2);
}

void Blend::init()
{
    for (ValueObject* object : audioSources->getList()->objects)
    {
        object->start(startTime);
    }

    position->start(startTime);
}

void Blend::fillBuffer(double* buffer, const unsigned int bufferLength)
{
    const std::vector<ValueObject*>& objects = audioSources->getList()->objects;

    if (objects.empty())
    {
        return;
    }

    double positionValue = fmin(1, fmax(0, position->getValue())) * (objects.size() - 1);

    const unsigned int startIndex = positionValue;

    positionValue -= startIndex;

    if (startIndex >= objects.size() - 1)
    {
        dynamic_cast<AudioSource*>(objects.back()->expandVariable())->fillBuffer(buffer, bufferLength);
    }

    else
    {
        std::fill(buffer1, buffer1 + bufferLength * utils->channels, 0);
        std::fill(buffer2, buffer2 + bufferLength * utils->channels, 0);

        dynamic_cast<AudioSource*>(objects[startIndex]->expandVariable())->fillBuffer(buffer1, bufferLength);
        dynamic_cast<AudioSource*>(objects[startIndex + 1]->expandVariable())->fillBuffer(buffer2, bufferLength);

        for (unsigned int i = 0; i < bufferLength * utils->channels; i++)
        {
            buffer[i] += (buffer1[i] + (buffer2[i] - buffer1[i]) * positionValue);
        }
    }
}
