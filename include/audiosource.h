#pragma once

#include <vector>
#include <string>

#include "AudioFile.h"

#include "parameter.h"
#include "effect.h"
#include "utils.h"

struct AudioSource : public Sync, public Object
{
    AudioSource();
    ~AudioSource();

    void fillBuffer(double* buffer, unsigned int bufferLength);

    virtual void prepareForEffects(unsigned int bufferLength) = 0;

    void addEffect(Effect* effect);
    void removeEffect(Effect* effect);

    double* effectBuffer;

    std::vector<Effect*> effects;

    ParameterController* volume = new ParameterController();
    ParameterController* pan = new ParameterController();
};

struct Oscillator : public AudioSource
{
    void prepareForEffects(unsigned int bufferLength) override;

    ParameterController* frequency;

    double phase = 0;
    double phaseDelta;
};

struct Sine : public Oscillator
{
    double getValue() override;
};

struct Square : public Oscillator
{
    double getValue() override;
};

struct Saw : public Oscillator
{
    double getValue() override;
};

struct Triangle : public Oscillator
{
    double getValue() override;
};

struct Noise : AudioSource
{
    void prepareForEffects(unsigned int bufferLength) override;

private:
    std::uniform_real_distribution<> udist = std::uniform_real_distribution<>(-1, 1);

};

struct Sample : AudioSource
{
    Sample(double volume, double pan, std::string path, int grains, bool looping);
    ~Sample();

    void prepareForEffects(unsigned int bufferLength) override;

    void start();

private:
    double* data;

    int length;

    std::vector<int> grains;

    bool looping;

};