#pragma once

#include <queue>

#include "parameter.h"
#include "utils.h"

struct Effect : public Sync, public Object
{
    virtual void apply(double* buffer, unsigned int bufferLength) = 0;

    ParameterController* mix = new ParameterController();
};

struct Delay : public Effect
{
    void apply(double* buffer, unsigned int bufferLength) override;

    ParameterController* delay = new ParameterController();
    ParameterController* feedback = new ParameterController();

private:
    std::queue<double> buffer;

    double bufferTime = 0;

};

struct LowPassFilter : public Effect
{
    void apply(double* buffer, unsigned int bufferLength) override;

    ParameterController* cutoff = new ParameterController();

private:
    double raw1[2] = { 0 };
    double raw2[2] = { 0 };
    double filtered1[2] = { 0 };
    double filtered2[2] = { 0 };

};