#pragma once

#include <queue>

#include "object.h"

struct Effect : public ValueObject
{
    virtual void apply(double* buffer);
};

struct EffectGroup : public Effect
{
    EffectGroup(ValueObject* mix, ValueObject* effects);
    ~EffectGroup();

    void apply(double* buffer) override;

protected:
    void init() override;

private:
    ValueObject* mix;
    ValueObject* effects;

    double* original;
    double* applied;

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

struct LowPass : public Effect
{
    LowPass(ValueObject* threshold);
    ~LowPass();

    void apply(double* buffer) override;

protected:
    void init() override;

private:
    ValueObject* threshold;

    double* raw;
    double* filtered;

};
