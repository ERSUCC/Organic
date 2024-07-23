#include "../include/audiosource.h"

AudioSource::AudioSource(ValueObject* volume, ValueObject* pan, ValueObject* effects) :
    volume(volume), pan(pan), effects(effects)
{
    effectBuffer = (double*)malloc(sizeof(double) * utils->bufferLength * utils->channels);
}

AudioSource::~AudioSource()
{
    free(effectBuffer);
}

void AudioSource::fillBuffer(double* buffer, const unsigned int bufferLength)
{
    if (enabled)
    {
        prepareForEffects(bufferLength);

        for (Effect* effect : getEffectsList())
        {
            effect->apply(effectBuffer, bufferLength);
        }

        for (unsigned int i = 0; i < bufferLength * utils->channels; i++)
        {
            buffer[i] += effectBuffer[i] * utils->volume;
        }
    }
}

std::vector<Effect*> AudioSource::getEffectsList() const
{
    ValueObject* current = effects;

    while (Variable* variable = dynamic_cast<Variable*>(current))
    {
        current = variable->value;
    }

    if (List<Effect>* list = dynamic_cast<List<Effect>*>(current))
    {
        return list->objects;
    }

    // potentially error if can't cast to effect

    if (Effect* effect = dynamic_cast<Effect*>(current))
    {
        return std::vector<Effect*> { effect };
    }

    return std::vector<Effect*>();
}

Oscillator::Oscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    AudioSource(volume, pan, effects), frequency(frequency) {}

void Oscillator::finishStart()
{
    volume->start(startTime);
    pan->start(startTime);
    frequency->start(startTime);

    for (Effect* effect : getEffectsList())
    {
        effect->start(startTime);
    }
}

void Oscillator::finishStop()
{
    volume->stop();
    pan->stop();
    frequency->stop();
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
    AudioSource(volume, pan, effects) {}

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

Sample::Sample(ValueObject* volume, ValueObject* pan, ValueObject* effects, std::string path, unsigned int grains, bool looping) :
    AudioSource(volume, pan, effects), grains(grains, 0), looping(looping)
{
    AudioFile<double> file(path);

    length = file.getNumSamplesPerChannel() * utils->channels;

    data = (double*)malloc(sizeof(double) * length);

    for (int i = 0; i < file.getNumSamplesPerChannel(); i++)
    {
        if (utils->channels == 1)
        {
            if (file.getNumChannels() == 1)
            {
                data[i] = file.samples[0][i];
            }

            else
            {
                data[i] = (file.samples[0][i] + file.samples[1][i]) / 2;
            }
        }

        else
        {
            if (file.getNumChannels() == 1)
            {
                data[i * 2] = file.samples[0][i];
                data[i * 2 + 1] = file.samples[0][i];
            }

            else
            {
                data[i * 2] = file.samples[0][i];
                data[i * 2 + 1] = file.samples[1][i];
            }
        }
    }
}

Sample::~Sample()
{
    free(data);
}

void Sample::finishStart()
{
    std::fill(grains.begin(), grains.end(), 0);

    for (Effect* effect : getEffectsList())
    {
        effect->start(startTime);
    }
}

void Sample::prepareForEffects(const unsigned int bufferLength)
{
    for (unsigned int i = 0; i < bufferLength * utils->channels; i++)
    {
        effectBuffer[i] = 0;
    }

    const double volumeValue = volume->getValue();
    const double panValue = pan->getValue();

    for (unsigned int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        for (unsigned int j = 0; j < grains.size(); j++)
        {
            if (grains[j] >= length && looping)
            {
                grains[j] = 0;
            }

            if (grains[j] < length)
            {
                if (utils->channels == 1)
                {
                    effectBuffer[i] += volume->getValue() * data[grains[j]++];
                }

                else
                {
                    const double value = volumeValue * data[grains[j]++];
                    const double value2 = volumeValue * data[grains[j]++];

                    effectBuffer[i] += value * (1 - panValue) / 2;
                    effectBuffer[i + 1] += value2 * (panValue + 1) / 2;
                }
            }
        }

        effectBuffer[i] /= grains.size();

        if (utils->channels == 2)
        {
            effectBuffer[i + 1] /= grains.size();
        }
    }
}
