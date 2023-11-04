#include "../include/effect.h"

Effect::Effect(double mix) : mix(mix) {}

Delay::Delay(double mix, double delay, double feedback) : Effect(mix), delay(delay), feedback(feedback) {}

void Delay::apply(double* buffer, unsigned int bufferLength)
{
    for (int i = 0; i < bufferLength * config->channels; i += config->channels)
    {
        if (config->channels == 1)
        {
            if (config->time >= bufferTime + delay)
            {
                buffer[i] += this->buffer.front() * mix * feedback;

                this->buffer.pop();

                bufferTime = config->time - delay;
            }

            this->buffer.push(buffer[i]);
        }

        else
        {
            if (config->time >= bufferTime + delay)
            {
                buffer[i] += this->buffer.front() * mix * feedback;

                this->buffer.pop();

                buffer[i + 1] += this->buffer.front() * mix * feedback;

                this->buffer.pop();

                bufferTime = config->time - delay;
            }

            this->buffer.push(buffer[i]);
            this->buffer.push(buffer[i + 1]);
        }
    }
}

LowPassFilter::LowPassFilter(double mix, double cutoff) : Effect(mix), cutoff(cutoff) {}

void LowPassFilter::apply(double* buffer, unsigned int bufferLength)
{
    double omega = tan(M_PI * cutoff / config->sampleRate);
    double omega2 = omega * omega;
    double c = 1 + 2 * cos(M_PI / 4) * omega + omega2;
    double a0 = omega2 / c;
    double a1 = 2 * a0;
    double b1 = 2 * (omega2 - 1) / c;
    double b2 = (1 - 2 * cos(M_PI / 4) * omega + omega2) / c;

    for (int i = 0; i < bufferLength * config->channels; i += config->channels)
    {
        double rawl = buffer[i];

        if (config->channels == 1)
        {
            buffer[i] = mix * (a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);

            raw2[0] = raw1[0];
            raw1[0] = rawl;

            filtered2[0] = filtered1[0];
            filtered1[0] = buffer[i];
        }

        else
        {
            double rawr = buffer[i + 1];

            buffer[i] = mix * (a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);
            buffer[i + 1] = mix * (a0 * buffer[i + 1] + a1 * raw1[1] + a0 * raw2[1] - b1 * filtered1[1] - b2 * filtered2[1]);

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