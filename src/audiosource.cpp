#include "../include/audiosource.h"

AudioSource::AudioSource(double volume, double pan) : volume(volume), pan(pan) {}

OscillatorAudioSource::OscillatorAudioSource(double volume, double pan, double frequency) : AudioSource(volume, pan), frequency(frequency) {}

void OscillatorAudioSource::fillBuffer(double* buffer, unsigned int bufferLength)
{
    phaseDelta = Constants::TWO_PI * frequency.value / Constants::SAMPLE_RATE;

    for (int i = 0; i < bufferLength; i++)
    {
        if (Constants::CHANNELS == 1)
        {
            buffer[i] += getValue();
        }

        else
        {
            double value = getValue();

            buffer[i * 2] += (value - pan) / 2;
            buffer[i * 2 + 1] += (value + pan) / 2;
        }

        phase += phaseDelta;
    }

    phase = fmod(phase, Constants::TWO_PI);
}

SineAudioSource::SineAudioSource(double volume, double pan, double frequency) : OscillatorAudioSource(volume, pan, frequency) {}

double SineAudioSource::getValue()
{
    return sin(phase);
}

SquareAudioSource::SquareAudioSource(double volume, double pan, double frequency) : OscillatorAudioSource(volume, pan, frequency) {}

double SquareAudioSource::getValue()
{
    if (sin(phase) > 0)
    {
        return 1;
    }

    return -1;
}

SawAudioSource::SawAudioSource(double volume, double pan, double frequency) : OscillatorAudioSource(volume, pan, frequency) {}

double SawAudioSource::getValue()
{
    return fmod(phase, Constants::TWO_PI) / M_PI - 1;
}