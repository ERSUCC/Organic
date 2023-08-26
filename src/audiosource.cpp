#include "../include/audiosource.h"

AudioSource::AudioSource(double volume, double pan) : volume(volume), pan(pan) {}

Oscillator::Oscillator(double volume, double pan, double frequency) : AudioSource(volume, pan), frequency(frequency) {}

void Oscillator::fillBuffer(double* buffer, unsigned int bufferLength)
{
    phaseDelta = Config::TWO_PI * frequency.value / Config::SAMPLE_RATE;

    for (int i = 0; i < bufferLength; i++)
    {
        if (Config::CHANNELS == 1)
        {
            buffer[i] += volume.value * getValue();
        }

        else
        {
            double value = getValue();

            buffer[i * 2] += volume.value * value * (1 - pan.value) / 2;
            buffer[i * 2 + 1] += volume.value * value * (pan.value + 1) / 2;
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
        return 1;
    }

    return -1;
}

Saw::Saw(double volume, double pan, double frequency) : Oscillator(volume, pan, frequency) {}

double Saw::getValue()
{
    return fmod(phase, Config::TWO_PI) / M_PI - 1;
}