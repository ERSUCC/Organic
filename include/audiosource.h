#pragma once

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <random>
#include <string>

#include "effect.h"
#include "object.h"
#include "resource.h"

namespace Engine {

struct AudioSource : public ValueObject
{
    virtual void fillBuffer(double* buffer);
};

struct SingleAudioSource : public AudioSource
{
    SingleAudioSource(ValueObject* volume, ValueObject* pan, ValueObject* effects);
    ~SingleAudioSource();

    void fillBuffer(double* buffer) override;

protected:
    virtual void prepareForEffects() = 0;

    double* effectBuffer;

    ValueObject* volume;
    ValueObject* pan;
    ValueObject* effects;

};

struct Phase : public ValueObject
{
    double getValue() override;

    void setDelta(double delta);

    void incrementPhase();

protected:
    void init() override;
    void reinit() override;

private:
    double phase = 0;
    double delta = 0;

};

struct Oscillator : public SingleAudioSource
{
    Oscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency);

protected:
    void init() override;

    void prepareForEffects() override;

    ValueObject* frequency;

    Phase* phase;

private:
    double lastVolume = 0;

};

struct Sine : public Oscillator
{
    Sine(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency);

    double getValue() override;
};

struct Square : public Oscillator
{
    Square(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency);

    double getValue() override;
};

struct Saw : public Oscillator
{
    Saw(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency);

    double getValue() override;
};

struct Triangle : public Oscillator
{
    Triangle(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency);

    double getValue() override;
};

struct CustomOscillator : public Oscillator
{
    CustomOscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency, ValueObject* waveform);

    double getValue() override;

protected:
    void init() override;

private:
    ValueObject* waveform;

};

struct Noise : public SingleAudioSource
{
    Noise(ValueObject* volume, ValueObject* pan, ValueObject* effects);

protected:
    void init() override;

    void prepareForEffects() override;

private:
    std::uniform_real_distribution<> udist = std::uniform_real_distribution<>(-1, 1);

};

struct Sample : public SingleAudioSource
{
    Sample(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource);

protected:
    void init() override;

    void prepareForEffects() override;

private:
    ValueObject* resource;

    unsigned int index;

};

struct ShapeCoordinator : public ValueObject
{
    double getValue() override;

    inline void setValue(const double value);

private:
    double value = 0;

};

struct Grain : public Sync
{
    Grain(ValueObject* resource, ValueObject* shape, ShapeCoordinator* coordinator, const unsigned int length);

    void apply(double* buffer);

    inline void setLength(const unsigned int length);

protected:
    void init() override;

private:
    unsigned int clampLength(const unsigned int max) const;
    unsigned int randomIndex(const unsigned int max) const;

    ValueObject* resource;
    ValueObject* shape;

    ShapeCoordinator* coordinator;

    unsigned int length;

    unsigned int currentIndex;
    unsigned int startIndex;

    bool firstInit = true;

};

struct GrainNode
{
    GrainNode(Grain* grain, GrainNode* prev, GrainNode* next);
    ~GrainNode();

    Grain* grain;

    GrainNode* prev;
    GrainNode* next;

    bool active = true;
};

struct GrainList : public Sync
{
    GrainList();

    void append(Grain* grain);

    void apply(double* buffer, const unsigned int grainLength, const unsigned int maxGrains);

    inline unsigned int getActiveLength() const;
    inline unsigned int getTotalLength() const;

private:
    GrainNode* head = new GrainNode(nullptr, nullptr, nullptr);
    GrainNode* tail = new GrainNode(nullptr, nullptr, nullptr);

    unsigned int activeLength = 0;
    unsigned int totalLength = 0;

};

struct Granulate : public SingleAudioSource
{
    Granulate(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource, ValueObject* grains, ValueObject* length, ValueObject* shape);

protected:
    void init() override;

    void prepareForEffects() override;

private:
    ValueObject* resource;
    ValueObject* grains;
    ValueObject* length;
    ValueObject* shape;

    ShapeCoordinator* coordinator = new ShapeCoordinator();

    GrainList* grainList = new GrainList();

};

struct Group : public AudioSource
{
    Group(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* sources);
    ~Group();

    void fillBuffer(double* buffer) override;

protected:
    void init() override;

private:
    ValueObject* volume;
    ValueObject* pan;
    ValueObject* effects;
    ValueObject* sources;

    double* effectBuffer;

};

}
