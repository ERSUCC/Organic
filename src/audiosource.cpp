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

void Noise::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
}

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

    if (utils->channels == 1)
    {
        effectBuffer[0] = volumeValue * resourceLeaf->samples[index];
    }

    else
    {
        effectBuffer[0] = volumeValue * resourceLeaf->samples[index] * (1 - panValue) / 2;
        effectBuffer[1] = volumeValue * resourceLeaf->samples[index + 1] * (panValue + 1) / 2;
    }

    index += utils->channels;

    if (index >= resourceLeaf->length)
    {
        index -= resourceLeaf->length;
    }
}

double ShapeCoordinator::getValue()
{
    return value;
}

void ShapeCoordinator::setValue(const double value)
{
    this->value = value;
}

Grain::Grain(ValueObject* resource, ValueObject* shape, ShapeCoordinator* coordinator, const unsigned int length) :
    resource(resource), shape(shape), coordinator(coordinator), length(length) {}

void Grain::apply(double* buffer)
{
    const Resource* resourceLeaf = resource->getLeafAs<Resource>();

    const unsigned int clamped = clampLength(resourceLeaf->length);

    coordinator->setValue((double)(currentIndex - startIndex) / clamped);

    const double shapeValue = shape->getValue();

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        buffer[i] += resourceLeaf->samples[currentIndex++] * shapeValue;
    }

    if (currentIndex >= startIndex + clamped)
    {
        stop();
    }
}

void Grain::setLength(const unsigned int length)
{
    this->length = length;
}

void Grain::init()
{
    const unsigned int maxLength = resource->getLeafAs<Resource>()->length;
    const unsigned int clamped = clampLength(maxLength);

    currentIndex = randomIndex(maxLength - clamped);
    startIndex = currentIndex;

    if (firstInit)
    {
        currentIndex += randomIndex(clamped);

        firstInit = false;
    }
}

unsigned int Grain::clampLength(const unsigned int max) const
{
    if (length > max)
    {
        return max;
    }

    return length;
}

unsigned int Grain::randomIndex(const unsigned int max) const
{
    return (std::uniform_int_distribution(0U, max)(utils->rng) / utils->channels) * utils->channels;
}

GrainNode::GrainNode(Grain* grain, GrainNode* prev, GrainNode* next) :
    grain(grain), prev(prev), next(next) {}

GrainNode::~GrainNode()
{
    delete grain;
}

GrainList::GrainList()
{
    head->next = tail;
    tail->prev = head;
}

void GrainList::append(Grain* grain)
{
    tail->prev->next = new GrainNode(grain, tail->prev, tail);
    tail->prev = tail->prev->next;

    activeLength++;
    totalLength++;
}

void GrainList::apply(double* buffer, const unsigned int grainLength, const unsigned int maxGrains)
{
    GrainNode* current = head->next;

    while (current != tail)
    {
        if (activeLength > maxGrains && current->active)
        {
            current->active = false;

            activeLength--;
        }

        current->grain->apply(buffer);

        if (!current->grain->enabled)
        {
            if (current->active)
            {
                current->grain->setLength(grainLength);
                current->grain->start(utils->time);
            }

            else
            {
                current->prev->next = current->next;
                current->next->prev = current->prev;

                totalLength--;

                GrainNode* old = current;

                current = current->prev;

                delete old;
            }
        }

        current = current->next;
    }
}

unsigned int GrainList::getActiveLength() const
{
    return activeLength;
}

unsigned int GrainList::getTotalLength() const
{
    return totalLength;
}

Granulate::Granulate(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource, ValueObject* grains, ValueObject* length, ValueObject* shape) :
    SingleAudioSource(volume, pan, effects), resource(resource), grains(grains), length(length), shape(shape)
{
    shape->getLeafAs<Lambda>()->setInputs({ coordinator });
}

void Granulate::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
    resource->start(startTime);
    grains->start(startTime);
    length->start(startTime);
    shape->start(startTime);
}

void Granulate::prepareForEffects()
{
    memset(effectBuffer, 0, sizeof(double) * utils->channels);

    const unsigned int lengthValue = utils->sampleRate * utils->channels * length->getValue() / 1000;
    const unsigned int grainsValue = grains->getValue();

    if (grainList->getActiveLength() < grainsValue)
    {
        const unsigned int count = grainsValue - grainList->getActiveLength();

        for (unsigned int i = 0; i < count; i++)
        {
            Grain* grain = new Grain(resource, shape, coordinator, lengthValue);

            grain->start(utils->time);

            grainList->append(grain);
        }
    }

    grainList->apply(effectBuffer, lengthValue, grainsValue);

    const double volumeValue = volume->getValue() / fmax(1.3 * sqrt(grainList->getTotalLength()), 1);

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
