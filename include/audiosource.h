#pragma once

#include <vector>

#include "constants.h"

class Envelope
{

public:
    Envelope(unsigned int attack, unsigned int decay, double sustain, unsigned int release, double maxAmplitude);

    void start(long long time);
    void stop(long long time);
    void update(long long time);
    void setValue(double* value);

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

};

struct EnvelopeLink
{
    Envelope* envelope;

    double* value;
};

class AudioSource
{

public:
    AudioSource(double volume);

    virtual void fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase) = 0;

    void addEnvelope(Envelope* envelope, double* value);
    
    void update(long long time);

    double volume;

private:
    std::vector<EnvelopeLink> envelopes;

};

class SineAudioSource : public AudioSource
{

public:
    SineAudioSource(double frequency, double volume);

    void fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase) override;

    double frequency;

};