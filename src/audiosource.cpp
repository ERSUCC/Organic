#include "../include/audiosource.h"

void AudioSource::fillBuffer(double* buffer) {}

SingleAudioSource::SingleAudioSource(ValueObject* volume, ValueObject* pan, ValueObject* effects) :
    volume(volume), pan(pan), effects(effects)
{
    effectBuffer = (double*)malloc(sizeof(double) * utils->channels);
}

SingleAudioSource::~SingleAudioSource()
{
    free(effectBuffer);
}

void SingleAudioSource::fillBuffer(double* buffer)
{
    prepareForEffects();

    for (ValueObject* effect : effects->getLeafAs<List>()->objects)
    {
        effect->getLeafAs<Effect>()->apply(effectBuffer);
    }

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        buffer[i] += effectBuffer[i];
    }
}

double Phase::getValue()
{
    return phase;
}

void Phase::setDelta(double delta)
{
    this->delta = delta;
}

void Phase::incrementPhase()
{
    phase += delta;

    if (phase > utils->twoPi)
    {
        phase -= utils->twoPi;
    }
}

void Phase::init()
{
    phase = 0;
}

void Phase::reinit()
{
    phase = 0;
}

Oscillator::Oscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    SingleAudioSource(volume, pan, effects), frequency(frequency), phase(new Phase()) {}

void Oscillator::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
    frequency->start(startTime);
    phase->start(startTime);
}

void Oscillator::prepareForEffects()
{
    phase->setDelta(utils->twoPi * frequency->getValue() / utils->sampleRate);

    const double volumeValue = volume->getValue();

    if (lastVolume == 0 && volumeValue != 0)
    {
        phase->repeat(utils->time);
    }

    lastVolume = volumeValue;

    const double panValue = pan->getValue();

    const double value = volumeValue * getValue();

    if (utils->channels == 1)
    {
        effectBuffer[0] = value;
    }

    else
    {
        effectBuffer[0] = value * (1 - panValue) / 2;
        effectBuffer[1] = value * (panValue + 1) / 2;
    }

    phase->incrementPhase();
}

Sine::Sine(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Sine::getValue()
{
    return sin(phase->getValue());
}

Square::Square(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Square::getValue()
{
    if (sin(phase->getValue()) > 0)
    {
        return -1;
    }

    return 1;
}

Saw::Saw(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Saw::getValue()
{
    return phase->getValue() / utils->pi - 1;
}

Triangle::Triangle(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Triangle::getValue()
{
    return 2 * asin(sin(phase->getValue())) / utils->pi;
}

CustomOscillator::CustomOscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency, ValueObject* waveform) :
    Oscillator(volume, pan, effects, frequency), waveform(waveform)
{
    waveform->getLeafAs<Lambda>()->setInputs({ phase });
}

double CustomOscillator::getValue()
{
    return waveform->getValue();
}

void CustomOscillator::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
    frequency->start(startTime);
    waveform->start(startTime);
    phase->start(startTime);
}

Noise::Noise(ValueObject* volume, ValueObject* pan, ValueObject* effects) :
    SingleAudioSource(volume, pan, effects) {}

void Noise::prepareForEffects()
{
    const double volumeValue = volume->getValue();
    const double panValue = pan->getValue();

    const double value = volumeValue * udist(utils->rng);

    if (utils->channels == 1)
    {
        effectBuffer[0] = value;
    }

    else
    {
        effectBuffer[0] = value * (1 - panValue) / 2;
        effectBuffer[1] = value * (panValue + 1) / 2;
    }
}

Sample::Sample(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource) :
    SingleAudioSource(volume, pan, effects), resource(resource) {}

void Sample::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
    resource->start(startTime);

    index = 0;
}

void Sample::prepareForEffects()
{
    const double volumeValue = volume->getValue();
    const double panValue = pan->getValue();

    const Resource* resourceLeaf = resource->getLeafAs<Resource>();

    const double value1 = volumeValue * resourceLeaf->samples[(unsigned int)floor(index)];
    double value2 = value1;

    if (resourceLeaf->channels == 2)
    {
        value2 = volumeValue * resourceLeaf->samples[(unsigned int)floor(index) + 1];
    }

    if (utils->channels == 1)
    {
        effectBuffer[0] = (value1 + value2) / 2;
    }

    else
    {
        effectBuffer[0] = value1 * (1 - panValue) / 2;
        effectBuffer[1] = value2 * (panValue + 1) / 2;
    }

    index += resourceLeaf->channels * (double)resourceLeaf->sampleRate / utils->sampleRate;

    if (index >= resourceLeaf->length)
    {
        index = fmod(index, resourceLeaf->length);
    }
}

Group::Group(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* sources) :
    volume(volume), pan(pan), effects(effects), sources(sources)
{
    effectBuffer = (double*)malloc(sizeof(double) * utils->channels);
}

Group::~Group()
{
    free(effectBuffer);
}

void Group::fillBuffer(double* buffer)
{
    memset(effectBuffer, 0, sizeof(double) * utils->channels);

    for (ValueObject* source : sources->getLeafAs<List>()->objects)
    {
        source->getLeafAs<AudioSource>()->fillBuffer(effectBuffer);
    }

    const double volumeValue = volume->getValue();

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        effectBuffer[i] *= volumeValue;
    }

    const double panValue = pan->getValue();

    if (utils->channels == 2)
    {
        effectBuffer[0] = effectBuffer[0] * (1 - panValue) / 2;
        effectBuffer[1] = effectBuffer[1] * (1 + panValue) / 2;
    }

    for (ValueObject* effect : effects->getLeafAs<List>()->objects)
    {
        effect->getLeafAs<Effect>()->apply(effectBuffer);
    }

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        buffer[i] += effectBuffer[i];
    }
}

void Group::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
    sources->start(startTime);
}
