#pragma once

#include <math.h>

#define TWO_PI (M_PI * 2)

class AudioSource
{

public:
    AudioSource(unsigned int sampleRate) : sampleRate(sampleRate) {};

    virtual void fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase) = 0;

protected:
    unsigned int sampleRate;

};

class TestAudioSource : public AudioSource
{

public:
    TestAudioSource(unsigned int sampleRate) : AudioSource(sampleRate) {};
    
    void fillBuffer(double* buffer, unsigned int bufferLength, unsigned int phase) override;

};