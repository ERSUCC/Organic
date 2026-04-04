#pragma once

#include <queue>

#include "object.h"

struct Effect : public ValueObject
{
    virtual void apply(double* buffer);
};

struct Delay : public Effect
{
    Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback);

    void apply(double* buffer) override;

protected:
    void init() override;

private:
    ValueObject* mix;
    ValueObject* delay;
    ValueObject* feedback;

    std::queue<double> delayBuffer;

};

struct LowPassFilter : public Effect
{
    LowPassFilter(ValueObject* mix, ValueObject* cutoff);

    void apply(double* buffer) override;

protected:
    void init() override;

private:
    ValueObject* mix;
    ValueObject* cutoff;

    double raw1[2] = { 0 };
    double raw2[2] = { 0 };
    double filtered1[2] = { 0 };
    double filtered2[2] = { 0 };

};

struct Comb : public Effect
{
    Comb(ValueObject* mix, ValueObject* delay, ValueObject* feedback);

    void apply(double* buffer) override;

protected:
    void init() override;

private:
    ValueObject* mix;
    ValueObject* delay;
    ValueObject* feedback;

    std::queue<double> delayBuffer;

};

struct AllPass : public Effect
{
    AllPass(ValueObject* mix, ValueObject* delay, ValueObject* feedback);

    void apply(double* buffer) override;

protected:
    void init() override;

private:
    ValueObject* mix;
    ValueObject* delay;
    ValueObject* feedback;

    std::queue<double> delayBuffer;

};
