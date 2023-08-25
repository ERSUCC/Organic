#include "../include/audiosource.h"

AdjustableProperty::AdjustableProperty(double value) : value(value) {}

Envelope::Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release, double floor, double ceiling) :
    attack(attack), decay(decay), sustain(sustain), release(release), floor(floor), ceiling(ceiling)
{
    start(0);
}

void Envelope::connectValue(double* value)
{
    connectedValues.push_back(value);
}

void Envelope::start(long long time)
{
    amplitude = floor;
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
            amplitude = floor + (ceiling - floor) * (time - startTime) / attack;
        }

        else if (time - startTime - attack < decay)
        {
            amplitude = ceiling - (ceiling - sustain) * (double)(time - startTime - attack) / decay;
        }

        else
        {
            amplitude = sustain;
        }
    }

    else
    {
        amplitude = fmax(floor, peak * (1 - (double)(time - stopTime) / release));
    }

    for (double* value : connectedValues)
    {
        *value = amplitude;
    }
}

AudioSource::AudioSource(double volume) : volume(volume) {}

OscillatorAudioSource::OscillatorAudioSource(double volume, double frequency) : AudioSource(volume), frequency(frequency) {}

void OscillatorAudioSource::fillBuffer(double* buffer, unsigned int bufferLength)
{
    phaseDelta = Constants::TWO_PI * frequency / Constants::SAMPLE_RATE;

    for (int i = 0; i < bufferLength; i++)
    {
        double value = volume * getValue() / Constants::CHANNELS;

        for (int j = 0; j < Constants::CHANNELS; j++)
        {
            buffer[i * Constants::CHANNELS + j] += value;
        }

        phase += phaseDelta;
    }

    phase = fmod(phase, Constants::TWO_PI);
}

SineAudioSource::SineAudioSource(double volume, double frequency) : OscillatorAudioSource(volume, frequency) {}

double SineAudioSource::getValue()
{
    return sin(phase);
}

SquareAudioSource::SquareAudioSource(double volume, double frequency) : OscillatorAudioSource(volume, frequency) {}

double SquareAudioSource::getValue()
{
    if (sin(phase) > 0)
    {
        return 1;
    }

    return -1;
}

SawAudioSource::SawAudioSource(double volume, double frequency) : OscillatorAudioSource(volume, frequency) {}

double SawAudioSource::getValue()
{
    return fmod(phase, Constants::TWO_PI) / M_PI - 1;
}