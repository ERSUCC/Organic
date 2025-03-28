#pragma once

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>

#include "effect.h"
#include "object.h"

struct AudioSource : public ValueObject
{
    virtual void fillBuffer(double* buffer, const unsigned int bufferLength);
};

struct SingleAudioSource : public AudioSource
{
    SingleAudioSource(ValueObject* volume, ValueObject* pan, ValueObject* effects);
    ~SingleAudioSource();

    void fillBuffer(double* buffer, const unsigned int bufferLength) override;

protected:
    virtual void prepareForEffects(const unsigned int bufferLength) = 0;

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

private:
    double phase = 0;
    double delta = 0;

};

struct Oscillator : public SingleAudioSource
{
    Oscillator(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* frequency);

protected:
    void init() override;

    void prepareForEffects(const unsigned int bufferLength) override;

    ValueObject* frequency;

    Phase* phase;

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
    void prepareForEffects(const unsigned int bufferLength) override;

private:
    std::uniform_real_distribution<> udist = std::uniform_real_distribution<>(-1, 1);

};

struct Sample : public SingleAudioSource
{
    Sample(ValueObject* volume, ValueObject* pan, ValueObject* effects, ValueObject* resource);

protected:
    void init() override;

    void prepareForEffects(const unsigned int bufferLength) override;

private:
    ValueObject* resource;

    double index;

};
