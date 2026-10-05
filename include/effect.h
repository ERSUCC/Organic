#pragma once

#include <cstring>
#include <queue>
#include <random>
#include <stddef.h>
#include <unordered_map>
#include <utility>

#include "object.h"

namespace Engine {

struct Effect : public ValueObject
{
    virtual void apply(ValueObject* source, double* buffer);
};

struct EffectGroup : public Effect
{
    EffectGroup(ValueObject* mix, ValueObject* effects);
    ~EffectGroup();

    void apply(ValueObject* source, double* buffer) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* mix;
    ValueObject* effects;

    double* original;
    double* applied;

};

struct Delay : public Effect
{
    Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback);
    ~Delay();

    void apply(ValueObject* source, double* buffer) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* mix;
    ValueObject* delay;
    ValueObject* feedback;

    std::unordered_map<ValueObject*, std::queue<double>> delayBuffers;

};

struct Comb : public Effect
{
    Comb(ValueObject* mix, ValueObject* delay, ValueObject* feedback);
    ~Comb();

    void apply(ValueObject* source, double* buffer) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* mix;
    ValueObject* delay;
    ValueObject* feedback;

    std::unordered_map<ValueObject*, std::queue<double>> delayBuffers;

};

struct AllPass : public Effect
{
    AllPass(ValueObject* mix, ValueObject* delay, ValueObject* feedback);
    ~AllPass();

    void apply(ValueObject* source, double* buffer) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* mix;
    ValueObject* delay;
    ValueObject* feedback;

    std::unordered_map<ValueObject*, std::queue<double>> delayBuffers;

};

struct History
{
    History();
    ~History();

    double* raw;
    double* filtered;
};

struct LowPass : public Effect
{
    LowPass(ValueObject* threshold);
    ~LowPass();

    void apply(ValueObject* source, double* buffer) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* threshold;

    std::unordered_map<ValueObject*, History*> histories;

};

struct RingBuffer
{
    RingBuffer(const size_t length);
    ~RingBuffer();

    void push(const double value);

    double value() const;

private:
    const size_t length;

    double* buffer;

    size_t front = 0;

};

struct DelayLine
{
    DelayLine(const size_t length);
    ~DelayLine();

    void push(const double value);

    double value();

private:
    RingBuffer* buffer;

    double raw[2] = { 0 };
    double filtered[2] = { 0 };

    double c0;
    double c1;
    double c2;

};

struct DelayMatrix
{
    DelayMatrix();
    ~DelayMatrix();

    void apply(double* buffer, const double feedbackValue, const double mixValue);

private:
    const double coeffs[256] =
    {
        1, -1, -1, -1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1,
        -1, 1, -1, -1, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1,
        -1, -1, 1, -1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1,
        -1, -1, -1, 1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1,
        -1, 1, 1, 1, 1, -1, -1, -1, -1, 1, 1, 1, -1, 1, 1, 1,
        1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1, 1, -1, 1, 1,
        1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, 1, 1, -1, 1,
        1, 1, 1, -1, -1, -1, -1, 1, 1, 1, 1, -1, 1, 1, 1, -1,
        -1, 1, 1, 1, -1, 1, 1, 1, 1, -1, -1, -1, -1, 1, 1, 1,
        1, -1, 1, 1, 1, -1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1,
        1, 1, -1, 1, 1, 1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1,
        1, 1, 1, -1, 1, 1, 1, -1, -1, -1, -1, 1, 1, 1, 1, -1,
        -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1, 1, -1, -1, -1,
        1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, 1, -1, 1, -1, -1,
        1, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1, 1, -1, -1, 1, -1,
        1, 1, 1, -1, 1, 1, 1, -1, 1, 1, 1, -1, -1, -1, -1, 1
    };

    Utils* utils;

    DelayLine** lines;

    double* values;

    double delayLength;

};

struct Reverb : public Effect
{
    Reverb(ValueObject* mix, ValueObject* length);
    ~Reverb();

    void apply(ValueObject* source, double* buffer) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* mix;
    ValueObject* length;

    std::unordered_map<ValueObject*, DelayMatrix*> matrices;

};

}
