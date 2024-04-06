#pragma once

#include <string>
#include <vector>

#include "../deps/AudioFile.h"

#include "controller.h"
#include "effect.h"
#include "management.h"
#include "object.h"

struct AudioSource : public ValueObject, public AudioSourceTrait
{
    AudioSource(ValueObject* volume, ValueObject* pan, List<Effect>* effects);
    ~AudioSource();

    void fillBuffer(double* buffer, const unsigned int bufferLength, const double masterVolume) override;

    ValueObject* volume;
    ValueObject* pan;

protected:
    virtual void prepareForEffects(const unsigned int bufferLength) = 0;

    double* effectBuffer;

    List<Effect>* effects;

};

struct Oscillator : public AudioSource
{
    Oscillator(ValueObject* volume, ValueObject* pan, List<Effect>* effects, ValueObject* frequency);

    ValueObject* frequency;

    double phase = 0;
    double phaseDelta;

protected:
    void finishStart() override;
    void finishStop() override;

    void prepareForEffects(const unsigned int bufferLength) override;

};

struct Sine : public Oscillator
{
    Sine(ValueObject* volume, ValueObject* pan, List<Effect>* effects, ValueObject* frequency);

    double getValue() override;
};

struct Square : public Oscillator
{
    Square(ValueObject* volume, ValueObject* pan, List<Effect>* effects, ValueObject* frequency);

    double getValue() override;
};

struct Saw : public Oscillator
{
    Saw(ValueObject* volume, ValueObject* pan, List<Effect>* effects, ValueObject* frequency);

    double getValue() override;
};

struct Triangle : public Oscillator
{
    Triangle(ValueObject* volume, ValueObject* pan, List<Effect>* effects, ValueObject* frequency);

    double getValue() override;
};

struct Noise : public AudioSource
{
    Noise(ValueObject* volume, ValueObject* pan, List<Effect>* effects);

protected:
    void prepareForEffects(const unsigned int bufferLength) override;

private:
    std::uniform_real_distribution<> udist = std::uniform_real_distribution<>(-1, 1);

};

struct Sample : public AudioSource
{
    Sample(ValueObject* volume, ValueObject* pan, List<Effect>* effects, std::string path, int grains, bool looping);
    ~Sample();

protected:
    void finishStart() override;

    void prepareForEffects(const unsigned int bufferLength) override;

private:
    double* data;

    int length;

    std::vector<int> grains;

    bool looping;

};
