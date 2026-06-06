#include "../include/effect.h"

void Effect::apply(double* buffer) {}

EffectGroup::EffectGroup(ValueObject* mix, ValueObject* effects) :
    mix(mix), effects(effects)
{
    original = (double*)malloc(sizeof(double) * utils->channels);
    applied = (double*)malloc(sizeof(double) * utils->channels);
}

EffectGroup::~EffectGroup()
{
    free(original);
    free(applied);
}

void EffectGroup::apply(double* buffer)
{
    memcpy(original, buffer, sizeof(double) * utils->channels);
    memset(buffer, 0, sizeof(double) * utils->channels);

    const double mixValue = mix->getValue();

    const std::vector<ValueObject*>& effectObjects = effects->getLeafAs<List>()->objects;

    for (ValueObject* effect : effectObjects)
    {
        memcpy(applied, original, sizeof(double) * utils->channels);

        static_cast<Effect*>(effect)->apply(applied);

        for (unsigned int i = 0; i < utils->channels; i++)
        {
            buffer[i] += applied[i] / effectObjects.size();
        }
    }

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        buffer[i] = original[i] * (1 - mixValue) + buffer[i] * mixValue;
    }
}

void EffectGroup::init()
{
    mix->start(startTime);
    effects->start(startTime);
}

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
            delayBuffer.push(buffer[i] - value * feedbackValue);

            buffer[i] = buffer[i] * (1 - mixValue) - value * feedbackValue * mixValue;
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

LowPass::LowPass(ValueObject* threshold) :
    threshold(threshold)
{
    raw = (double*)calloc(utils->channels * 2, sizeof(double));
    filtered = (double*)calloc(utils->channels * 2, sizeof(double));
}

LowPass::~LowPass()
{
    free(raw);
    free(filtered);
}

void LowPass::apply(double* buffer)
{
    const double omega = tan(utils->pi * threshold->getValue() / utils->sampleRate);
    const double omega2 = omega * omega;
    const double c = 1 + sqrt(2) * omega + omega2;
    const double a = omega2 / c;
    const double b1 = 2 * (omega2 - 1) / c;
    const double b2 = (1 - sqrt(2) * omega + omega2) / c;

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        const double value = buffer[i];

        buffer[i] = a * (buffer[i] +  raw[i] * 2 + raw[i + utils->channels]) - b1 * filtered[i] - b2 * filtered[i + utils->channels];

        raw[i + utils->channels] = raw[i];
        raw[i] = value;

        filtered[i + utils->channels] = filtered[i];
        filtered[i] = buffer[i];
    }
}

void LowPass::init()
{
    threshold->start(startTime);
}

RingBuffer::RingBuffer(const size_t length) :
    length(length)
{
    buffer = (double*)calloc(length, sizeof(double));
}

RingBuffer::~RingBuffer()
{
    free(buffer);
}

void RingBuffer::push(const double value)
{
    buffer[front++] = value;

    if (front >= length)
    {
        front = 0;
    }
}

double RingBuffer::value() const
{
    return buffer[front];
}

DelayLine::DelayLine(const size_t length)
{
    buffer = new RingBuffer(length);

    const Utils* utils = Utils::get();

    const double omega = tan(utils->pi * 5000 / utils->sampleRate);
    const double omega2 = omega * omega;
    const double c = 1 + sqrt(2) * omega + omega2;

    c0 = omega2 / c;
    c1 = 2 * (omega2 - 1) / c;
    c2 = (1 - sqrt(2) * omega + omega2) / c;
}

DelayLine::~DelayLine()
{
    delete buffer;
}

void DelayLine::push(const double value)
{
    buffer->push(value);
}

double DelayLine::value()
{
    const double input = buffer->value();
    const double output = c0 * (input + raw[0] * 2 + raw[1]) - c1 * filtered[0] - c2 * filtered[1];

    raw[1] = raw[0];
    raw[0] = input;

    filtered[1] = filtered[0];
    filtered[0] = output;

    return output;
}

DelayMatrix::DelayMatrix()
{
    utils = Utils::get();

    lines = (DelayLine**)malloc(sizeof(DelayLine*) * 16 * utils->channels);

    std::uniform_int_distribution udist(0U, 1000U);

    for (size_t i = 0; i < 16; i++)
    {
        const size_t length = 2000U + i * 1000U + udist(utils->rng);

        for (unsigned int j = 0; j < utils->channels; j++)
        {
            lines[i * utils->channels + j] = new DelayLine(length);
        }

        if (i == 15)
        {
            delayLength = length * 1000 / utils->sampleRate;
        }
    }

    values = (double*)malloc(sizeof(double) * 16);
}

DelayMatrix::~DelayMatrix()
{
    for (size_t i = 0; i < 16 * utils->channels; i++)
    {
        delete lines[i];
    }

    free(lines);
    free(values);
}

void DelayMatrix::apply(double* buffer, const double lengthValue, const double mixValue)
{
    const double feedbackValue = exp(-3 * delayLength / lengthValue);

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        for (size_t j = 0; j < 16; j++)
        {
            values[j] = lines[j * utils->channels + i]->value();
        }

        double sum = 0;

        for (size_t j = 0; j < 16; j++)
        {
            double mult = 0;

            for (size_t k = 0; k < 16; k++)
            {
                mult += values[k] * coeffs[j * 16 + k] * 0.25;
            }

            mult *= feedbackValue;

            lines[j * utils->channels + i]->push(buffer[i] + mult);

            sum += mult;
        }

        buffer[i] = buffer[i] * (1 - mixValue) + sum * mixValue / 16;
    }
}

Reverb::Reverb(ValueObject* mix, ValueObject* length) :
    mix(mix), length(length) {}

void Reverb::apply(double* buffer)
{
    matrix->apply(buffer, length->getValue(), mix->getValue());
}

void Reverb::init()
{
    mix->start(startTime);
    length->start(startTime);
}
