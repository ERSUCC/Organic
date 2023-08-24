#include "../include/audiosource.h"

Envelope::Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release) : attack(attack), decay(decay), sustain(sustain), release(release) {}

AudioSource::AudioSource(double volume, unsigned int attack, unsigned int decay, double sustain, unsigned int release) : volume(volume), envelope(attack, decay, sustain, release) {}

void AudioSource::trigger(long long time)
{
    amplitude = 0;
    start = time;
    hold = true;
}

void AudioSource::update(long long time)
{
    if (hold)
    {
        if (time - start < envelope.attack)
        {
            amplitude = volume * (time - start) / envelope.attack;
        }

        else if (time - start - envelope.attack < envelope.decay)
        {
            amplitude = envelope.sustain + volume * (1 - envelope.sustain) * (1 - (double)(time - start - envelope.attack) / envelope.decay);
        }

        else
        {
            amplitude = volume * envelope.sustain;
        }
    }

    else if (amplitude > 0)
    {
        amplitude = peak * (1 - (double)(time - start) / envelope.release);
    }

    else
    {
        amplitude = 0;
    }
}

void AudioSource::release(long long time)
{
    peak = amplitude;
    start = time;
    hold = false;
}

SineAudioSource::SineAudioSource(double frequency, double volume, unsigned int attack, unsigned int decay, double sustain, unsigned int release) :
    AudioSource(volume, attack, decay, sustain, release), frequency(frequency) {}

void SineAudioSource::fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase)
{
    for (int i = 0; i < bufferLength; i++)
    {
        double value = 0.5 * amplitude * sin(Constants::TWO_PI * frequency * (phase + i) / Constants::SAMPLE_RATE);

        for (int j = 0; j < Constants::CHANNELS; j++)
        {
            buffer[i * Constants::CHANNELS + j] += value;
        }
    }
}