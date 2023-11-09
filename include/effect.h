#pragma once

#include <queue>

#include "parameter.h"
#include "utils.h"

struct Effect : public Sync, public Object
{
    Effect(double mix);

    virtual void apply(double* buffer, unsigned int bufferLength) = 0;

    Parameter mix;
};

struct Delay : public Effect
{
    Delay(double mix, double delay, double feedback);

    void apply(double* buffer, unsigned int bufferLength) override;

    Parameter delay;
    Parameter feedback;

private:
    std::queue<double> buffer;

    double bufferTime = 0;

};

struct LowPassFilter : public Effect
{
    LowPassFilter(double mix, double cutoff);

    void apply(double* buffer, unsigned int bufferLength) override;

    Parameter cutoff;

private:
    double raw1[2] = { 0 };
    double raw2[2] = { 0 };
    double filtered1[2] = { 0 };
    double filtered2[2] = { 0 };

};