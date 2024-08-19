#pragma once

#include <queue>

#include "object.h"

struct Effect : public ValueObject
{
    Effect(ValueObject* mix);

    virtual void apply(double* buffer, const unsigned int bufferLength) = 0;

    ValueObject* mix;
};

struct Delay : public Effect
{
    Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback);

    void apply(double* buffer, const unsigned int bufferLength) override;

    ValueObject* delay;
    ValueObject* feedback;

protected:
    void init() override;

private:
    std::queue<double> buffer;

};

struct LowPassFilter : public Effect
{
    LowPassFilter(ValueObject* mix, ValueObject* cutoff);
    
    void apply(double* buffer, const unsigned int bufferLength) override;

    ValueObject* cutoff;

protected:
    void init() override;

private:
    double raw1[2] = { 0 };
    double raw2[2] = { 0 };
    double filtered1[2] = { 0 };
    double filtered2[2] = { 0 };

};
