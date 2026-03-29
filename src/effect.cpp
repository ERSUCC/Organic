#include "../include/effect.h"

void Effect::apply(double* buffer) {}

Delay::Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback) :
    mix(mix), delay(delay), feedback(feedback) {}

void Delay::apply(double* buffer)
{
    const size_t delayFrames = utils->channels * utils->sampleRate * delay->getValue() / 1000;

    while (delayBuffer.size() > delayFrames)
    {
        delayBuffer.pop();
    }

    const double mixValue = mix->getValue();
    const double feedbackValue = feedback->getValue();

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        if (delayFrames > 0 && delayBuffer.size() >= delayFrames)
        {
            const double value = delayBuffer.front() * feedbackValue;

            delayBuffer.pop();
            delayBuffer.push(buffer[i] + value);

            buffer[i] += value * mixValue;
        }

        else
        {
            delayBuffer.push(buffer[i]);
        }
    }
}

void Delay::init()
{
    mix->start(startTime);
    delay->start(startTime);
    feedback->start(startTime);
}

LowPassFilter::LowPassFilter(ValueObject* mix, ValueObject* cutoff) :
    mix(mix), cutoff(cutoff) {}

void LowPassFilter::apply(double* buffer)
{
    const double omega = tan(utils->pi * cutoff->getValue() / utils->sampleRate);
    const double omega2 = omega * omega;
    const double c = 1 + 2 * cos(utils->pi / 4) * omega + omega2;
    const double a0 = omega2 / c;
    const double a1 = 2 * a0;
    const double b1 = 2 * (omega2 - 1) / c;
    const double b2 = (1 - 2 * cos(utils->pi / 4) * omega + omega2) / c;

    const double mixValue = mix->getValue();

    const double rawl = buffer[0];

    if (utils->channels == 1)
    {
        buffer[0] = mixValue * (a0 * buffer[0] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);

        raw2[0] = raw1[0];
        raw1[0] = rawl;

        filtered2[0] = filtered1[0];
        filtered1[0] = buffer[0];
    }

    else
    {
        const double rawr = buffer[1];

        buffer[0] = mixValue * (a0 * buffer[0] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);
        buffer[1] = mixValue * (a0 * buffer[1] + a1 * raw1[1] + a0 * raw2[1] - b1 * filtered1[1] - b2 * filtered2[1]);

        raw2[0] = raw1[0];
        raw2[1] = raw1[1];
        raw1[0] = rawl;
        raw1[1] = rawr;

        filtered2[0] = filtered1[0];
        filtered2[1] = filtered1[1];
        filtered1[0] = buffer[0];
        filtered1[1] = buffer[1];
    }
}

void LowPassFilter::init()
{
    mix->start(startTime);
    cutoff->start(startTime);
}

Comb::Comb(ValueObject* mix, ValueObject* delay, ValueObject* feedback) :
    mix(mix), delay(delay), feedback(feedback) {}

void Comb::apply(double* buffer)
{
    const size_t delayFrames = utils->channels * utils->sampleRate * delay->getValue() / 1000;

    while (delayBuffer.size() > delayFrames)
    {
        delayBuffer.pop();
    }

    const double mixValue = mix->getValue();
    const double feedbackValue = feedback->getValue();

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        if (delayFrames > 0 && delayBuffer.size() >= delayFrames)
        {
            const double value = delayBuffer.front();

            delayBuffer.pop();
            delayBuffer.push(buffer[i] * (1 - feedbackValue) + value * feedbackValue);

            buffer[i] = buffer[i] * (1 - mixValue) + value * mixValue;
        }

        else
        {
            delayBuffer.push(buffer[i]);
        }
    }
}

void Comb::init()
{
    mix->start(startTime);
    delay->start(startTime);
    feedback->start(startTime);
}

AllPass::AllPass(ValueObject* mix, ValueObject* delay, ValueObject* feedback) :
    mix(mix), delay(delay), feedback(feedback) {}

void AllPass::apply(double* buffer)
{
    const size_t delayFrames = utils->channels * utils->sampleRate * delay->getValue() / 1000;

    while (delayBuffer.size() > delayFrames)
    {
        delayBuffer.pop();
    }

    const double mixValue = mix->getValue();
    const double feedbackValue = feedback->getValue();

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        double value = buffer[i] * feedbackValue;

        if (delayFrames > 0 && delayBuffer.size() >= delayFrames)
        {
            value += delayBuffer.front();

            delayBuffer.pop();
        }

        delayBuffer.push(buffer[i] - value * feedbackValue);

        buffer[i] = buffer[i] * (1 - mixValue) + value * mixValue;
    }
}

void AllPass::init()
{
    mix->start(startTime);
    delay->start(startTime);
    feedback->start(startTime);
}
