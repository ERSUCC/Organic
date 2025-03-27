#pragma once

#include <queue>

#include "object.h"

struct Effect : public ValueObject
{
    virtual void apply(double* buffer, const unsigned int bufferLength);
};

struct Delay : public Effect
{
    Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback);

    void apply(double* buffer, const unsigned int bufferLength) override;

    ValueObject* mix;
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

    ValueObject* mix;
    ValueObject* cutoff;

protected:
    void init() override;

private:
    double raw1[2] = { 0 };
    double raw2[2] = { 0 };
    double filtered1[2] = { 0 };
    double filtered2[2] = { 0 };

};
