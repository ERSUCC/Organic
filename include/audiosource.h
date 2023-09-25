#pragma once

#define _USE_MATH_DEFINES

#include <vector>

#include "config.h"
#include "parameter.h"
#include "effect.h"

struct AudioSource
{
    AudioSource(double volume, double pan);
    ~AudioSource();

    void fillBuffer(double* buffer, unsigned int bufferLength, double time);

    virtual void prepareForEffects(unsigned int bufferLength, double time) = 0;

    void addEffect(Effect* effect);
    void removeEffect(Effect* effect);

    double* effectBuffer;

    std::vector<Effect*> effects;

    Parameter volume;
    Parameter pan;
};

struct Oscillator : public AudioSource
{
    Oscillator(double volume, double pan, double frequency);

    void prepareForEffects(unsigned int bufferLength, double time) override;

    virtual double getValue() = 0;

    Parameter frequency;

    double phase = 0;
    double phaseDelta;
};

struct Sine : public Oscillator
{
    Sine(double volume, double pan, double frequency);

    double getValue() override;
};

struct Square : public Oscillator
{
    Square(double volume, double pan, double frequency);

    double getValue() override;
};

struct Saw : public Oscillator
{
    Saw(double volume, double pan, double frequency);

    double getValue() override;
};

struct Triangle : public Oscillator
{
    Triangle(double volume, double pan, double frequency);

    double getValue() override;
};

struct Noise : AudioSource
{
    Noise(double volume, double pan);

    void prepareForEffects(unsigned int bufferLength, double time) override;

private:
    std::uniform_real_distribution<> udist = std::uniform_real_distribution<>(-1, 1);

};