#pragma once

#include "constants.h"
#include "parameter.h"

struct AudioSource
{
    AudioSource(double volume, double pan);

    virtual void fillBuffer(double* buffer, unsigned int bufferLength) = 0;

    Parameter volume;
    Parameter pan;
};

struct Oscillator : public AudioSource
{
    Oscillator(double volume, double pan, double frequency);

    void fillBuffer(double* buffer, unsigned int bufferLength) override;

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