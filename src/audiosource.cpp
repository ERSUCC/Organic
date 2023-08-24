#include "../include/audiosource.h"

void TestAudioSource::fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase)
{
    for (int i = 0; i < bufferLength; i++)
    {
        double value = 0.5 * sin(TWO_PI * 440 * (phase + i) / sampleRate);

        buffer[i * 2] += value;
        buffer[i * 2 + 1] += value;
    }
}