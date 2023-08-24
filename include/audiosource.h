#pragma once

#include "constants.h"

struct Envelope
{
    Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release);

    unsigned int attack;
    unsigned int decay;
    double sustain;
    unsigned int release;
};

class AudioSource
{

public:
    AudioSource(double volume, unsigned int attack, unsigned int decay, double sustain, unsigned int release);

    virtual void fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase) = 0;
    
    void trigger(long long time);
    void update(long long time);
    void release(long long time);

protected:
    double volume;
    double amplitude = 0;
    double peak;

    Envelope envelope;

    long long start = 0;

    bool hold = false;

};

class SineAudioSource : public AudioSource
{

public:
    SineAudioSource(double frequency, double volume, unsigned int attack, unsigned int decay, double sustain, unsigned int release);

    void fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase) override;

private:
    double frequency;

};