#pragma once

#include <vector>

#include "constants.h"

class Envelope
{

public:
    Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release, double floor, double ceiling);

    void connectValue(double* value);

    void start(long long time);
    void stop(long long time);
    void update(long long time);

private:
    unsigned int attack;
    unsigned int decay;
    double sustain;
    unsigned int release;

    double amplitude;
    double peak;
    double floor;
    double ceiling;

    long long startTime;
    long long stopTime;

    bool hold;

    std::vector<double*> connectedValues;

};

class AudioSource
{

public:
    AudioSource(double volume);

    virtual void fillBuffer(double* buffer, unsigned int bufferLength) = 0;

    double volume;

};

class OscillatorAudioSource : public AudioSource
{

public:
    OscillatorAudioSource(double volume, double frequency);

    void fillBuffer(double* buffer, unsigned int bufferLength) override;

    virtual double getValue() = 0;

    double frequency;

    double phase = 0;
    double phaseDelta;

};

class SineAudioSource : public OscillatorAudioSource
{

public:
    SineAudioSource(double volume, double frequency);

    double getValue() override;

};

class SquareAudioSource : public OscillatorAudioSource
{

public:
    SquareAudioSource(double volume, double frequency);

    double getValue() override;

};

class SawAudioSource : public OscillatorAudioSource
{

public:
    SawAudioSource(double volume, double frequency);

    double getValue() override;

};