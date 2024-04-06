#pragma once

#include <queue>

#include "object.h"

struct Effect : public Sync
{
    Effect(ValueObject* mix);

    virtual void apply(double* buffer, unsigned int bufferLength) = 0;

    ValueObject* mix;
};

struct Delay : public Effect
{
    Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback);

    void apply(double* buffer, unsigned int bufferLength) override;

    ValueObject* delay;
    ValueObject* feedback;

protected:
    void finishStart() override;
    void finishStop() override;

private:
    std::queue<double> buffer;

    double bufferTime = 0;

};

struct LowPassFilter : public Effect
{
    LowPassFilter(ValueObject* mix, ValueObject* cutoff);
    
    void apply(double* buffer, unsigned int bufferLength) override;

    ValueObject* cutoff;

protected:
    void finishStart() override;
    void finishStop() override;

private:
    double raw1[2] = { 0 };
    double raw2[2] = { 0 };
    double filtered1[2] = { 0 };
    double filtered2[2] = { 0 };

};
