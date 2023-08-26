#pragma once

#include "constants.h"
#include "parameter.h"

class AudioSource
{

public:
    AudioSource(double volume, double pan);

    virtual void fillBuffer(double* buffer, unsigned int bufferLength) = 0;

    double volume;
    double pan;

};

class OscillatorAudioSource : public AudioSource
{

public:
    OscillatorAudioSource(double volume, double pan, double frequency);

    void fillBuffer(double* buffer, unsigned int bufferLength) override;

    virtual double getValue() = 0;

    Parameter frequency;

    double phase = 0;
    double phaseDelta;

};

class SineAudioSource : public OscillatorAudioSource
{

public:
    SineAudioSource(double volume, double pan, double frequency);

    double getValue() override;

};

class SquareAudioSource : public OscillatorAudioSource
{

public:
    SquareAudioSource(double volume, double pan, double frequency);

    double getValue() override;

};

class SawAudioSource : public OscillatorAudioSource
{

public:
    SawAudioSource(double volume, double pan, double frequency);

    double getValue() override;

};