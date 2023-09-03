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