#pragma once

#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

struct Utils
{
    static Utils* get();

    static void warning(std::string message);
    static void error(std::string message);

    const unsigned int sampleRate = 44100;
    const unsigned int channels = 2;
    const unsigned int bufferLength = 128;

    const double twoPi = M_PI * 2;
    double volume = 0.25;
    double time = 0;

    std::mt19937 rng;

private:
    Utils();

    static Utils* instance;
    
};

struct AssignableObject
{
    virtual ~AssignableObject();
};

struct Sync : public AssignableObject
{
    Sync();

protected:
    Utils* utils;

};

struct ValueObject : public Sync
{
    void start();
    void stop();

    double getValue();

    double startTime = 0;

    bool enabled = false;

protected:
    virtual void finishStart();
    virtual void finishStop();

    virtual double getValueUnchecked() = 0;

};

struct Variable : public ValueObject
{
    ValueObject* value;

protected:
    void finishStart() override;
    void finishStop() override;

    double getValueUnchecked() override;

};