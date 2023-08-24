#include "../include/audiosource.h"

Envelope::Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release, double maxAmplitude) :
    attack(attack), decay(decay), sustain(sustain), release(release), maxAmplitude(maxAmplitude)
{
    start(0);
}

void Envelope::connectValue(double* value)
{
    connectedValues.push_back(value);
}

void Envelope::start(long long time)
{
    amplitude = 0;
    startTime = time;
    hold = true;
}

void Envelope::stop(long long time)
{
    peak = amplitude;
    stopTime = time;
    hold = false;
}

void Envelope::update(long long time)
{
    if (hold)
    {
        if (time - startTime < attack)
        {
            amplitude = maxAmplitude * (time - startTime) / attack;
        }

        else if (time - startTime - attack < decay)
        {
            amplitude = sustain + maxAmplitude * (1 - sustain) * (1 - (double)(time - startTime - attack) / decay);
        }

        else
        {
            amplitude = maxAmplitude * sustain;
        }
    }

    else if (amplitude > 0)
    {
        amplitude = peak * (1 - (double)(time - stopTime) / release);
    }

    else
    {
        amplitude = 0;
    }

    for (double* value : connectedValues)
    {
        *value = amplitude;
    }
}

AudioSource::AudioSource(double volume) : volume(volume) {}

SineAudioSource::SineAudioSource(double frequency, double volume) : AudioSource(volume), frequency(frequency) {}

void SineAudioSource::fillBuffer(double* buffer, unsigned int bufferLength)
{
    phaseDelta = Constants::TWO_PI * frequency / Constants::SAMPLE_RATE;

    for (int i = 0; i < bufferLength; i++)
    {
        double value = volume * sin(phase) / Constants::CHANNELS;

        for (int j = 0; j < Constants::CHANNELS; j++)
        {
            buffer[i * Constants::CHANNELS + j] += value;
        }

        phase += phaseDelta;
    }

    phase = fmod(phase, Constants::TWO_PI);
}