#include "../include/effect.h"

Delay::Delay(double delay, double feedback) : delay(delay), feedback(feedback) {}

void Delay::apply(double* buffer, unsigned int bufferLength, double time)
{
    for (int i = 0; i < bufferLength * Config::CHANNELS; i += Config::CHANNELS)
    {
        if (Config::CHANNELS == 1)
        {
            if (time >= bufferTime + delay.value)
            {
                buffer[i] += this->buffer.front() * feedback.value;

                this->buffer.pop();

                bufferTime = time - delay.value;
            }

            this->buffer.push(buffer[i]);
        }

        else
        {
            if (time >= bufferTime + delay.value)
            {
                buffer[i] += this->buffer.front() * feedback.value;

                this->buffer.pop();

                buffer[i + 1] += this->buffer.front() * feedback.value;

                this->buffer.pop();

                bufferTime = time - delay.value;
            }

            this->buffer.push(buffer[i]);
            this->buffer.push(buffer[i + 1]);
        }
    }
}

LowPassFilter::LowPassFilter(double cutoff) : cutoff(cutoff) {}

void LowPassFilter::apply(double* buffer, unsigned int bufferLength, double time)
{
    double omega = tan(M_PI * cutoff.value / Config::SAMPLE_RATE);
    double omega2 = omega * omega;
    double c = 1 + 2 * cos(M_PI / 4) * omega + omega2;
    double a0 = omega2 / c;
    double a1 = 2 * a0;
    double b1 = 2 * (omega2 - 1) / c;
    double b2 = (1 - 2 * cos(M_PI / 4) * omega + omega2) / c;

    for (int i = 0; i < bufferLength * Config::CHANNELS; i += Config::CHANNELS)
    {
        double rawl = buffer[i];

        if (Config::CHANNELS == 1)
        {
            buffer[i] = a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0];

            raw2[0] = raw1[0];
            raw1[0] = rawl;

            filtered2[0] = filtered1[0];
            filtered1[0] = buffer[i];
        }

        else
        {
            double rawr = buffer[i + 1];

            buffer[i] = a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0];
            buffer[i + 1] = a0 * buffer[i + 1] + a1 * raw1[1] + a0 * raw2[1] - b1 * filtered1[1] - b2 * filtered2[1];

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