#include "../include/effect.h"

void Delay::apply(double* buffer, unsigned int bufferLength)
{
    for (int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        if (utils->channels == 1)
        {
            if (utils->time >= bufferTime + delay->getValue())
            {
                buffer[i] += this->buffer.front() * mix->getValue() * feedback->getValue();

                this->buffer.pop();

                bufferTime = utils->time - delay->getValue();
            }

            this->buffer.push(buffer[i]);
        }

        else
        {
            if (utils->time >= bufferTime + delay->getValue())
            {
                buffer[i] += this->buffer.front() * mix->getValue() * feedback->getValue();

                this->buffer.pop();

                buffer[i + 1] += this->buffer.front() * mix->getValue() * feedback->getValue();

                this->buffer.pop();

                bufferTime = utils->time - delay->getValue();
            }

            this->buffer.push(buffer[i]);
            this->buffer.push(buffer[i + 1]);
        }
    }
}

void LowPassFilter::apply(double* buffer, unsigned int bufferLength)
{
    double omega = tan(M_PI * cutoff->getValue() / utils->sampleRate);
    double omega2 = omega * omega;
    double c = 1 + 2 * cos(M_PI / 4) * omega + omega2;
    double a0 = omega2 / c;
    double a1 = 2 * a0;
    double b1 = 2 * (omega2 - 1) / c;
    double b2 = (1 - 2 * cos(M_PI / 4) * omega + omega2) / c;

    for (int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        double rawl = buffer[i];

        if (utils->channels == 1)
        {
            buffer[i] = mix->getValue() * (a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);

            raw2[0] = raw1[0];
            raw1[0] = rawl;

            filtered2[0] = filtered1[0];
            filtered1[0] = buffer[i];
        }

        else
        {
            double rawr = buffer[i + 1];

            buffer[i] = mix->getValue() * (a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);
            buffer[i + 1] = mix->getValue() * (a0 * buffer[i + 1] + a1 * raw1[1] + a0 * raw2[1] - b1 * filtered1[1] - b2 * filtered2[1]);

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
