#pragma once

#include <vector>
#include <string>

#include "AudioFile.h"

#include "parameter.h"
#include "effect.h"
#include "utils.h"

struct AudioSource : public ValueObject
{
    AudioSource();
    ~AudioSource();

    void fillBuffer(double* buffer, unsigned int bufferLength);

    void addEffect(Effect* effect);
    void removeEffect(Effect* effect);

    ValueObject* volume;
    ValueObject* pan;

protected:
    virtual void prepareForEffects(unsigned int bufferLength) = 0;

    double* effectBuffer;

    std::vector<Effect*> effects;

};

struct Oscillator : public AudioSource
{
    ValueObject* frequency;

    double phase = 0;
    double phaseDelta;

protected:
    void finishStart() override;
    void finishStop() override;

    void prepareForEffects(unsigned int bufferLength) override;

};

struct Sine : public Oscillator
{

protected:
    double getValueUnchecked() override;
    
};

struct Square : public Oscillator
{

protected:
    double getValueUnchecked() override;
    
};

struct Saw : public Oscillator
{

protected:
    double getValueUnchecked() override;
    
};

struct Triangle : public Oscillator
{

protected:
    double getValueUnchecked() override;
    
};

struct Noise : AudioSource
{

protected:
    void prepareForEffects(unsigned int bufferLength) override;

private:
    std::uniform_real_distribution<> udist = std::uniform_real_distribution<>(-1, 1);

};

struct Sample : AudioSource
{
    Sample(double volume, double pan, std::string path, int grains, bool looping);
    ~Sample();

protected:
    void finishStart() override;

    void prepareForEffects(unsigned int bufferLength) override;

private:
    double* data;

    int length;

    std::vector<int> grains;

    bool looping;

};