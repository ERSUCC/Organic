#pragma once

#include <queue>

#include "parameter.h"
#include "utils.h"

struct Effect : public Sync
{
    virtual void apply(double* buffer, unsigned int bufferLength) = 0;

    ValueObject* mix;
};

struct Delay : public Effect
{
    void apply(double* buffer, unsigned int bufferLength) override;

    ValueObject* delay;
    ValueObject* feedback;

private:
    std::queue<double> buffer;

    double bufferTime = 0;

};

struct LowPassFilter : public Effect
{
    void apply(double* buffer, unsigned int bufferLength) override;

    ValueObject* cutoff;

private:
    double raw1[2] = { 0 };
    double raw2[2] = { 0 };
    double filtered1[2] = { 0 };
    double filtered2[2] = { 0 };

};