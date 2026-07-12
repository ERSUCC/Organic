#include "../include/audiosource.h"

using namespace Engine;

void AudioSource::fillBuffer(double* buffer) {}

SingleAudioSource::SingleAudioSource(ValueObject* volume, ValueObject* pan, ValueObject* effects) :
    volume(volume), pan(pan), effects(effects)
{
    effectBuffer = (double*)malloc(sizeof(double) * utils->channels);
}

SingleAudioSource::~SingleAudioSource()
{
    free(effectBuffer);

    delete volume;
    delete pan;
    delete effects;
}

void SingleAudioSource::fillBuffer(double* buffer)
{
    prepareForEffects();

    for (ValueObject* effect : effects->getLeafAs<List>()->objects)
    {
        effect->getLeafAs<Effect>()->apply(effectBuffer);
    }

    for (size_t i = 0; i < utils->channels; i++)
    {
        buffer[i] += effectBuffer[i];
    }
}

double Phase::getValue()
{
    return phase;
}

void Phase::setDelta(const double delta)
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

Oscillator::~Oscillator()
{
    delete frequency;
    delete phase;
}

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

CustomOscillator::~CustomOscillator()
{
    delete waveform;
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

Sample::~Sample()
{
    delete resource;
}

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

Grain::Grain(ValueObject* resource, ValueObject* shape, ShapeCoordinator* coordinator, const size_t length) :
    resource(resource), shape(shape), coordinator(coordinator), length(length) {}

Grain::~Grain()
{
    delete resource;
    delete shape;
    delete coordinator;
}

void Grain::apply(double* buffer)
{
    const Resource* resourceLeaf = resource->getLeafAs<Resource>();

    const size_t clamped = clampLength(resourceLeaf->length);

    coordinator->setValue((double)(currentIndex - startIndex) / clamped);

    const double shapeValue = shape->getValue();

    for (size_t i = 0; i < utils->channels; i++)
    {
        buffer[i] += resourceLeaf->samples[currentIndex++] * shapeValue;
    }

    if (currentIndex >= startIndex + clamped)
    {
        stop();
    }
}

void Grain::setLength(const size_t length)
{
    this->length = length;
}

void Grain::init()
{
    const size_t maxLength = resource->getLeafAs<Resource>()->length;
    const size_t clamped = clampLength(maxLength);

    currentIndex = randomIndex(maxLength - clamped);
    startIndex = currentIndex;

    if (firstInit)
    {
        currentIndex += randomIndex(clamped);

        firstInit = false;
    }
}

size_t Grain::clampLength(const size_t max) const
{
    if (length > max)
    {
        return max;
    }

    return length;
}

size_t Grain::randomIndex(const size_t max) const
{
    return (std::uniform_int_distribution<size_t>(0, max)(utils->rng) / utils->channels) * utils->channels;
}

GrainNode::GrainNode(Grain* grain, GrainNode* prev, GrainNode* next) :
    grain(grain), prev(prev), next(next) {}

GrainNode::~GrainNode()
{
    if (grain)
    {
        delete grain;
    }
}

GrainList::GrainList()
{
    head->next = tail;
    tail->prev = head;
}

GrainList::~GrainList()
{
    GrainNode* current = head;

    while (current != tail)
    {
        GrainNode* next = current->next;

        delete current;

        current = next;
    }

    delete tail;
}

void GrainList::append(Grain* grain)
{
    tail->prev->next = new GrainNode(grain, tail->prev, tail);
    tail->prev = tail->prev->next;

    activeLength++;
    totalLength++;
}

void GrainList::apply(double* buffer, const size_t grainLength, const size_t maxGrains)
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

size_t GrainList::getActiveLength() const
{
    return activeLength;
}

size_t GrainList::getTotalLength() const
{
    return totalLength;
}

Granulate::Granulate(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource, ValueObject* grains, ValueObject* length, ValueObject* shape) :
    SingleAudioSource(volume, pan, effects), resource(resource), grains(grains), length(length), shape(shape)
{
    shape->getLeafAs<Lambda>()->setInputs({ coordinator });
}

Granulate::~Granulate()
{
    delete resource;
    delete grains;
    delete length;
    delete shape;
    delete coordinator;
    delete grainList;
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

    const size_t lengthValue = utils->sampleRate * utils->channels * length->getValue() / 1000;
    const size_t grainsValue = grains->getValue();

    if (grainList->getActiveLength() < grainsValue)
    {
        const size_t count = grainsValue - grainList->getActiveLength();

        for (size_t i = 0; i < count; i++)
        {
            Grain* grain = new Grain(resource, shape, coordinator, lengthValue);

            grain->start(utils->time);

            grainList->append(grain);
        }
    }

    grainList->apply(effectBuffer, lengthValue, grainsValue);

    const double volumeValue = volume->getValue() / fmax(1.3 * sqrt(grainList->getTotalLength()), 1);

    for (size_t i = 0; i < utils->channels; i++)
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

    delete volume;
    delete pan;
    delete effects;
    delete sources;
}

void Group::fillBuffer(double* buffer)
{
    memset(effectBuffer, 0, sizeof(double) * utils->channels);

    for (ValueObject* source : sources->getLeafAs<List>()->objects)
    {
        source->getLeafAs<AudioSource>()->fillBuffer(effectBuffer);
    }

    const double volumeValue = volume->getValue();

    for (size_t i = 0; i < utils->channels; i++)
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

    for (size_t i = 0; i < utils->channels; i++)
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
