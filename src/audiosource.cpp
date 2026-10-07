#include "../include/audiosource.h"

using namespace Engine;

AudioSource::AudioSource(ValueObject* volume, ValueObject* pan, ValueObject* effects) :
    volume(volume), pan(pan), effects(new MultiList(effects))
{
    effectBuffer = (double*)malloc(sizeof(double) * utils->channels);
}

AudioSource::AudioSource() :
    AudioSource(new ValueObject(), new ValueObject(), new ValueObject()) {}

AudioSource::~AudioSource()
{
    delete volume;
    delete pan;
    delete effects;

    free(effectBuffer);
}

void AudioSource::fillBuffer(double* buffer)
{
    for (ValueObject* object : effects->getLeafAs<List>()->objects)
    {
        object->getLeafAs<Effect>()->apply(this, effectBuffer);
    }

    for (size_t i = 0; i < utils->channels; i++)
    {
        buffer[i] += effectBuffer[i];
    }
}

double Phase::getValue() const
{
    return phase;
}

void Phase::setDelta(const double delta)
{
    this->delta = delta;
}

void Phase::init()
{
    phase = 0;
}

void Phase::reinit()
{
    phase = 0;
}

void Phase::compute()
{
    if (utils->time > lastUpdate)
    {
        phase += delta;

        if (phase > utils->twoPi)
        {
            phase -= utils->twoPi;
        }

        lastUpdate = utils->time;
    }
}

Oscillator::Oscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    AudioSource(volume, pan, effects), frequency(frequency), phase(new Phase()) {}

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

void Oscillator::compute()
{
    volume->update();
    pan->update();
    effects->update();
    frequency->update();

    const double frequencyValue = frequency->getValue();

    if (frequencyValue == 0)
    {
        memset(effectBuffer, 0, sizeof(double) * utils->channels);

        return;
    }

    phase->setDelta(utils->twoPi * frequencyValue / utils->sampleRate);
    phase->update();

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
}

Sine::Sine(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Sine::getValue() const
{
    return sin(phase->getValue());
}

Square::Square(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Square::getValue() const
{
    const double phaseNorm = phase->getValue() / utils->twoPi;
    const double spread = frequency->getValue() / utils->sampleRate;

    if (phaseNorm < spread)
    {
        const double offset = phaseNorm / spread;

        return offset * (2 - offset);
    }

    if (phaseNorm > 0.5 - spread && phaseNorm < 0.5)
    {
        const double offset = (phaseNorm - 0.5) / spread;

        return -offset * (offset + 2);
    }

    if (phaseNorm > 0.5 && phaseNorm < 0.5 + spread)
    {
        const double offset = (phaseNorm - 0.5) / spread;

        return -offset * (2 - offset);
    }

    if (phaseNorm > 1 - spread)
    {
        const double offset = (phaseNorm - 1) / spread;

        return offset * (offset + 2);
    }

    if (phaseNorm < 0.5)
    {
        return 1;
    }

    return -1;
}

Saw::Saw(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Saw::getValue() const
{
    const double phaseNorm = phase->getValue() / utils->twoPi;
    const double spread = frequency->getValue() / utils->sampleRate;

    if (phaseNorm < spread)
    {
        const double offset = phaseNorm / spread;

        return phaseNorm * 2 - offset * (2 - offset);
    }

    if (phaseNorm > 1 - spread)
    {
        const double offset = (phaseNorm - 1) / spread;

        return phaseNorm * 2 - offset * (offset + 2) - 2;
    }

    return phaseNorm * 2 - 1;
}

Triangle::Triangle(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency) :
    Oscillator(volume, pan, effects, frequency) {}

double Triangle::getValue() const
{
    return 2 * asin(sin(phase->getValue())) / utils->pi;
}

CustomOscillator::CustomOscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency, Lambda* waveform) :
    Oscillator(volume, pan, effects, frequency), waveform(waveform) {}

CustomOscillator::~CustomOscillator()
{
    delete waveform;
}

double CustomOscillator::getValue() const
{
    waveform->update();

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

    waveform->setInput("phase", phase);
}

Noise::Noise(ValueObject* volume, ValueObject* pan, ValueObject* effects) :
    AudioSource(volume, pan, effects) {}

void Noise::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
}

void Noise::compute()
{
    volume->update();
    pan->update();
    effects->update();

    const double value = volume->getValue() * udist(utils->rng);
    const double panValue = pan->getValue();

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

Sample::Sample(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource, ValueObject* length) :
    AudioSource(volume, pan, effects), resource(resource), length(length) {}

Sample::~Sample()
{
    delete resource;
    delete length;
}

void Sample::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
    resource->start(startTime);
    length->start(startTime);

    index = 0;
}

void Sample::compute()
{
    volume->update();
    pan->update();
    effects->update();
    resource->update();
    length->update();

    const double volumeValue = volume->getValue();
    const double panValue = pan->getValue();

    const Resource* resourceLeaf = resource->getLeafAs<Resource>();

    if (index < resourceLeaf->length)
    {
        if (utils->channels == 1)
        {
            effectBuffer[0] = volumeValue * resourceLeaf->samples[index];
        }

        else
        {
            effectBuffer[0] = volumeValue * resourceLeaf->samples[index] * (1 - panValue) / 2;
            effectBuffer[1] = volumeValue * resourceLeaf->samples[index + 1] * (panValue + 1) / 2;
        }
    }

    index += utils->channels;

    const double lengthValue = length->getValue();

    if (lengthValue == 0 && index >= resourceLeaf->length)
    {
        stop(startTime + 1000.0 * (resourceLeaf->length / utils->channels) / utils->sampleRate);
    }

    else if (lengthValue > 0 && 1000.0 * (index / utils->channels) / utils->sampleRate >= lengthValue)
    {
        stop(startTime + lengthValue);
    }
}

double ShapeCoordinator::getValue() const
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
        stop(0);
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

Granulate::Granulate(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource, ValueObject* grains, ValueObject* length, Lambda* shape) :
    AudioSource(volume, pan, effects), resource(resource), grains(grains), length(length), shape(shape) {}

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

    shape->setInput("position", coordinator);
}

void Granulate::compute()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
    resource->start(startTime);
    grains->start(startTime);
    length->start(startTime);
    shape->start(startTime);

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
    AudioSource(volume, pan, effects), sources(new MultiList(sources)) {}

Group::~Group()
{
    delete sources;
}

void Group::init()
{
    volume->start(startTime);
    pan->start(startTime);
    effects->start(startTime);
    sources->start(startTime);
}

void Group::compute()
{
    volume->update();
    pan->update();
    effects->update();
    sources->update();

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
}
