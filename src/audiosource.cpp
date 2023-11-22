#include "../include/audiosource.h"

AudioSource::AudioSource()
{
    effectBuffer = (double*)malloc(sizeof(double) * utils->bufferLength * utils->channels);
}

AudioSource::~AudioSource()
{
    free(effectBuffer);
}

void AudioSource::fillBuffer(double* buffer, unsigned int bufferLength)
{
    prepareForEffects(bufferLength);

    for (Effect* effect : effects)
    {
        effect->apply(effectBuffer, bufferLength);
    }

    for (int i = 0; i < bufferLength * utils->channels; i++)
    {
        buffer[i] += effectBuffer[i];
    }
}

void AudioSource::addEffect(Effect* effect)
{
    effects.push_back(effect);
}

void AudioSource::removeEffect(Effect* effect)
{
    effects.erase(std::find(effects.begin(), effects.end(), effect));
}

void Oscillator::prepareForEffects(unsigned int bufferLength)
{
    phaseDelta = utils->twoPi * frequency->getValue() / utils->sampleRate;

    for (int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        if (utils->channels == 1)
        {
            effectBuffer[i] = volume->getValue() * getValue();
        }

        else
        {
            double value = volume->getValue() * getValue();

            effectBuffer[i] = value * (1 - pan->getValue()) / 2;
            effectBuffer[i + 1] = value * (pan->getValue() + 1) / 2;
        }

        phase += phaseDelta;
    }

    phase = fmod(phase, utils->twoPi);
}

double Sine::getValue()
{
    return sin(phase);
}

double Square::getValue()
{
    if (sin(phase) > 0)
    {
        return -1;
    }

    return 1;
}

double Saw::getValue()
{
    return fmod(phase, utils->twoPi) / M_PI - 1;
}

double Triangle::getValue()
{
    if (sin(phase) < 0)
    {
        return -fmod(phase - M_PI, M_PI) * 2 / M_PI + 1;
    }

    return fmod(phase, M_PI) * 2 / M_PI - 1;
}

void Noise::prepareForEffects(unsigned int bufferLength)
{
    for (int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        if (utils->channels == 1)
        {
            effectBuffer[i] = volume->getValue() * udist(utils->rng);
        }

        else
        {
            double value = volume->getValue() * udist(utils->rng);

            effectBuffer[i] = value * (1 - pan->getValue()) / 2;
            effectBuffer[i + 1] = value * (pan->getValue() + 1) / 2;
        }
    }
}

Sample::Sample(double volume, double pan, std::string path, int grains, bool looping) : grains(grains), looping(looping)
{
    for (int i = 0; i < grains; i++)
    {
        this->grains[i] = 0;
    }

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

void Sample::prepareForEffects(unsigned int bufferLength)
{
    for (int i = 0; i < bufferLength * utils->channels; i++)
    {
        effectBuffer[i] = 0;
    }

    for (int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        for (int j = 0; j < grains.size(); j++)
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
                    double value = volume->getValue() * data[grains[j]++];
                    double value2 = volume->getValue() * data[grains[j]++];

                    effectBuffer[i] += value * (1 - pan->getValue()) / 2;
                    effectBuffer[i + 1] += value2 * (pan->getValue() + 1) / 2;
                }
            }
        }
    }

    for (int i = 0; i < bufferLength * utils->channels; i++)
    {
        effectBuffer[i] /= grains.size();
    }
}

void Sample::start()
{
    for (int i = 0; i < grains.size(); i++)
    {
        grains[i] = 0;
    }
}