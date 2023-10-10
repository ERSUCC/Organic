#include "../include/audiosource.h"

AudioSource::AudioSource(double volume, double pan) : volume(volume), pan(pan)
{
    effectBuffer = (double*)malloc(sizeof(double) * Config::BUFFER_LENGTH * Config::CHANNELS);
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

    for (int i = 0; i < bufferLength * Config::CHANNELS; i++)
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

Oscillator::Oscillator(double volume, double pan, double frequency) : AudioSource(volume, pan), frequency(frequency) {}

void Oscillator::prepareForEffects(unsigned int bufferLength)
{
    phaseDelta = Config::TWO_PI * frequency.value / Config::SAMPLE_RATE;

    for (int i = 0; i < bufferLength * Config::CHANNELS; i += Config::CHANNELS)
    {
        if (Config::CHANNELS == 1)
        {
            effectBuffer[i] = volume.value * getValue();
        }

        else
        {
            double value = volume.value * getValue();

            effectBuffer[i] = value * (1 - pan.value) / 2;
            effectBuffer[i + 1] = value * (pan.value + 1) / 2;
        }

        phase += phaseDelta;
    }

    phase = fmod(phase, Config::TWO_PI);
}

Sine::Sine(double volume, double pan, double frequency) : Oscillator(volume, pan, frequency) {}

double Sine::getValue()
{
    return sin(phase);
}

Square::Square(double volume, double pan, double frequency) : Oscillator(volume, pan, frequency) {}

double Square::getValue()
{
    if (sin(phase) > 0)
    {
        return -1;
    }

    return 1;
}

Saw::Saw(double volume, double pan, double frequency) : Oscillator(volume, pan, frequency) {}

double Saw::getValue()
{
    return fmod(phase, Config::TWO_PI) / M_PI - 1;
}

Triangle::Triangle(double volume, double pan, double frequency) : Oscillator(volume, pan, frequency) {}

double Triangle::getValue()
{
    if (sin(phase) < 0)
    {
        return -fmod(phase - M_PI, M_PI) * 2 / M_PI + 1;
    }

    return fmod(phase, M_PI) * 2 / M_PI - 1;
}

Noise::Noise(double volume, double pan) : AudioSource(volume, pan) {}

void Noise::prepareForEffects(unsigned int bufferLength)
{
    for (int i = 0; i < bufferLength * Config::CHANNELS; i += Config::CHANNELS)
    {
        if (Config::CHANNELS == 1)
        {
            effectBuffer[i] = volume.value * udist(Config::RNG);
        }

        else
        {
            double value = volume.value * udist(Config::RNG);

            effectBuffer[i] = value * (1 - pan.value) / 2;
            effectBuffer[i + 1] = value * (pan.value + 1) / 2;
        }
    }
}

Sample::Sample(double volume, double pan, std::string path, int grains, bool looping) : AudioSource(volume, pan), grains(grains), looping(looping)
{
    for (int i = 0; i < grains; i++)
    {
        this->grains[i] = 0;
    }

    AudioFile<double> file(path);

    length = file.getNumSamplesPerChannel() * Config::CHANNELS;

    data = (double*)malloc(sizeof(double) * length);

    for (int i = 0; i < file.getNumSamplesPerChannel(); i++)
    {
        if (Config::CHANNELS == 1)
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
    for (int i = 0; i < bufferLength * Config::CHANNELS; i++)
    {
        effectBuffer[i] = 0;
    }

    for (int i = 0; i < bufferLength * Config::CHANNELS; i += Config::CHANNELS)
    {
        for (int j = 0; j < grains.size(); j++)
        {
            if (grains[j] >= length && looping)
            {
                grains[j] = 0;
            }

            if (grains[j] < length)
            {
                if (Config::CHANNELS == 1)
                {
                    effectBuffer[i] += volume.value * data[grains[j]++];
                }

                else
                {
                    double value = volume.value * data[grains[j]++];
                    double value2 = volume.value * data[grains[j]++];

                    effectBuffer[i] += value * (1 - pan.value) / 2;
                    effectBuffer[i + 1] += value2 * (pan.value + 1) / 2;
                }
            }
        }
    }

    for (int i = 0; i < bufferLength * Config::CHANNELS; i++)
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