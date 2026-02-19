#pragma once

#include <complex>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "object.h"

struct Effect : public ValueObject
{
    virtual void apply(double* buffer, const unsigned int bufferLength);
};

struct Delay : public Effect
{
    Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback);

    void apply(double* buffer, const unsigned int bufferLength) override;

    ValueObject* mix;
    ValueObject* delay;
    ValueObject* feedback;

protected:
    void init() override;

private:
    std::queue<double> buffer;

};

struct LowPassFilter : public Effect
{
    LowPassFilter(ValueObject* mix, ValueObject* cutoff);

    void apply(double* buffer, const unsigned int bufferLength) override;

    ValueObject* mix;
    ValueObject* cutoff;

protected:
    void init() override;

private:
    double raw1[2] = { 0 };
    double raw2[2] = { 0 };
    double filtered1[2] = { 0 };
    double filtered2[2] = { 0 };

};

struct Buffer
{
    Buffer(const size_t length);
    ~Buffer();

    inline void write(const double* source, const size_t length);
    inline void read(double* dest, const size_t offset, const size_t length) const;

    inline void push(const double value);

    inline size_t size() const;

private:
    const size_t length;

    size_t end = 0;

    double* buffer;

};

struct RingBuffer
{
    RingBuffer(const size_t length);
    ~RingBuffer();

    inline void push(const double value);
    inline void add(const size_t offset, const double value);

    inline double pop();

private:
    const size_t length;

    double* buffer;

    size_t front = 0;
    size_t back = 0;
    size_t size = 0;

};

struct Executable
{
    virtual void execute() = 0;
};

struct ExecutorThread
{
    ExecutorThread(const size_t maxScheduled);
    ~ExecutorThread();

    void schedule(Executable* executable);
    void wait();

private:
    const size_t maxScheduled;

    std::thread thread;

    std::mutex lock;
    std::condition_variable signal;

    Executable** scheduled;

    size_t end = 0;

    bool executing = true;

};

struct ExecutorPool
{
    ExecutorPool(const size_t numThreads, const size_t maxScheduled);
    ~ExecutorPool();

    void schedule(Executable* executable);
    void wait();

private:
    const size_t numThreads;

    ExecutorThread** threads;

    size_t next = 0;

};

struct Convolver : public Executable
{
    Convolver(const size_t length, const size_t offset, const double* impulse, const Buffer* input, RingBuffer* output);
    ~Convolver();

    void execute() override;

private:
    void fft(const double* start, const size_t length, const size_t step, std::complex<double>* result) const;
    void ifft(const std::complex<double>* start, const size_t length, const size_t step, std::complex<double>* result) const;

    const size_t length;
    const size_t offset;

    const Buffer* input;

    RingBuffer* output;

    std::complex<double>* powers;

    double* inputBuffer;

    std::complex<double>* buffer1;
    std::complex<double>* buffer2;

    double* overlap;

    std::complex<double>* impulse;

    size_t last = 0;

};

struct ConvolverStream
{
    ConvolverStream(const std::vector<size_t>& segments, const double* impulse);
    ~ConvolverStream();

    inline void write(const double* source, const size_t length, const size_t stride);
    inline void execute();
    inline void read(double* dest, const size_t length, const size_t stride, const double mix);

private:
    const std::vector<size_t> segments;

    Convolver** convolvers;

    Buffer* input;

    RingBuffer* output;

    ExecutorPool* executor;

};

struct Reverb : public Effect
{
    Reverb(ValueObject* mix, ValueObject* response);
    ~Reverb();

    void apply(double* buffer, const unsigned int bufferLength) override;

    ValueObject* mix;
    ValueObject* response;

protected:
    void init() override;

private:
    std::vector<size_t> segments;

    size_t offset = 0;

    ConvolverStream** streams;

};
