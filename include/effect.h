#pragma once

#include <queue>

#include "parameter.h"

struct Effect
{
    virtual void apply(double* buffer, unsigned int bufferLength, double time) = 0;
};

struct Delay : public Effect
{
    Delay(double delay, double feedback);

    void apply(double* buffer, unsigned int bufferLength, double time) override;

    Parameter delay;
    Parameter feedback;

private:
    std::queue<double> buffer;

    double bufferTime = 0;

};