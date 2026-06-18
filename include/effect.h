#pragma once

#include <cstring>
#include <queue>
#include <random>

#include "object.h"

namespace Engine {

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

    void apply(double* buffer) override;

protected:
    void init() override;

private:
    ValueObject* mix;
    ValueObject* length;

    DelayMatrix* matrix = new DelayMatrix();

};

}
