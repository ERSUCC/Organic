#include "../include/effect.h"

Effect::Effect(ValueObject* mix) :
    mix(mix) {}

Delay::Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback) :
    Effect(mix), delay(delay), feedback(feedback) {}

void Delay::apply(double* buffer, const unsigned int bufferLength)
{
    const double delayLength = utils->sampleRate * delay->getValue() / 1000;

    if (delayLength == 0)
    {
        return;
    }

    for (unsigned int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        if (utils->channels == 1)
        {
            if (this->buffer.size() > delayLength)
            {
                buffer[i] += this->buffer.front() * mix->getValue() * feedback->getValue();

                this->buffer.pop();
            }

            this->buffer.push(buffer[i]);
        }

        else
        {
            if (this->buffer.size() > delayLength * 2)
            {
                const double mixValue = mix->getValue();
                const double feedbackValue = feedback->getValue();

                buffer[i] += this->buffer.front() * mixValue * feedbackValue;

                this->buffer.pop();

                buffer[i + 1] += this->buffer.front() * mixValue * feedbackValue;

                this->buffer.pop();
            }

            this->buffer.push(buffer[i]);
            this->buffer.push(buffer[i + 1]);
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
    Effect(mix), cutoff(cutoff) {}

void LowPassFilter::apply(double* buffer, unsigned int bufferLength)
{
    const double omega = tan(utils->pi * cutoff->getValue() / utils->sampleRate);
    const double omega2 = omega * omega;
    const double c = 1 + 2 * cos(utils->pi / 4) * omega + omega2;
    const double a0 = omega2 / c;
    const double a1 = 2 * a0;
    const double b1 = 2 * (omega2 - 1) / c;
    const double b2 = (1 - 2 * cos(utils->pi / 4) * omega + omega2) / c;

    for (unsigned int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        const double mixValue = mix->getValue();

        const double rawl = buffer[i];

        if (utils->channels == 1)
        {
            buffer[i] = mixValue * (a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);

            raw2[0] = raw1[0];
            raw1[0] = rawl;

            filtered2[0] = filtered1[0];
            filtered1[0] = buffer[i];
        }

        else
        {
            const double rawr = buffer[i + 1];

            buffer[i] = mixValue * (a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);
            buffer[i + 1] = mixValue * (a0 * buffer[i + 1] + a1 * raw1[1] + a0 * raw2[1] - b1 * filtered1[1] - b2 * filtered2[1]);

            raw2[0] = raw1[0];
            raw2[1] = raw1[1];
            raw1[0] = rawl;
            raw1[1] = rawr;

            filtered2[0] = filtered1[0];
            filtered2[1] = filtered1[1];
            filtered1[0] = buffer[i];
            filtered1[1] = buffer[i + 1];
        }
    }
}

void LowPassFilter::init()
{
    mix->start(startTime);
    cutoff->start(startTime);
}
