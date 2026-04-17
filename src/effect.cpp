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
