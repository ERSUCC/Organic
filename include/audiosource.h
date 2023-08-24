#pragma once

#include <vector>

#include "constants.h"

class Envelope
{

public:
    Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release, double maxAmplitude);

    void connectValue(double* value);

    void start(long long time);
    void stop(long long time);
    void update(long long time);

private:
    unsigned int attack;
    unsigned int decay;
    double sustain;
    unsigned int release;

    double maxAmplitude;
    double amplitude;
    double peak;

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

class SineAudioSource : public AudioSource
{

public:
    SineAudioSource(double frequency, double volume);

    void fillBuffer(double* buffer, unsigned int bufferLength) override;

    double frequency;

private:
    double phase = 0;
    double phaseDelta;

};