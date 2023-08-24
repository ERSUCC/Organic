#include "../include/audiosource.h"

Envelope::Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release, double maxAmplitude) :
    attack(attack), decay(decay), sustain(sustain), release(release), maxAmplitude(maxAmplitude) {}

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
}

void Envelope::setValue(double* value)
{
    *value = amplitude;
}

AudioSource::AudioSource(double volume) : volume(volume) {}

void AudioSource::addEnvelope(Envelope* envelope, double* value)
{
    envelopes.push_back({ envelope, value });
}

void AudioSource::update(long long time)
{
    for (EnvelopeLink link : envelopes)
    {
        link.envelope->setValue(link.value);
    }
}

SineAudioSource::SineAudioSource(double frequency, double volume) : AudioSource(volume), frequency(frequency) {}

void SineAudioSource::fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase)
{
    for (int i = 0; i < bufferLength; i++)
    {
        double value = 0.5 * volume * sin(Constants::TWO_PI * frequency * (phase + i) / Constants::SAMPLE_RATE);

        for (int j = 0; j < Constants::CHANNELS; j++)
        {
            buffer[i * Constants::CHANNELS + j] += value;
        }
    }
}