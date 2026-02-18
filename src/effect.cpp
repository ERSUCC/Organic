#include "../include/effect.h"

void Effect::apply(double* buffer, const unsigned int bufferLength) {}

Delay::Delay(ValueObject* mix, ValueObject* delay, ValueObject* feedback) :
    mix(mix), delay(delay), feedback(feedback) {}

void Delay::apply(double* buffer, const unsigned int bufferLength)
{
    const double delayLength = utils->sampleRate * delay->getValue() / 1000;

    if (delayLength == 0)
    {
        return;
    }

    for (unsigned int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        if (utils->channels == 1)
        {
            if (this->buffer.size() > delayLength)
            {
                buffer[i] += this->buffer.front() * mix->getValue() * feedback->getValue();

                this->buffer.pop();
            }

            this->buffer.push(buffer[i]);
        }

        else
        {
            if (this->buffer.size() > delayLength * 2)
            {
                const double mixValue = mix->getValue();
                const double feedbackValue = feedback->getValue();

                buffer[i] += this->buffer.front() * mixValue * feedbackValue;

                this->buffer.pop();

                buffer[i + 1] += this->buffer.front() * mixValue * feedbackValue;

                this->buffer.pop();
            }

            this->buffer.push(buffer[i]);
            this->buffer.push(buffer[i + 1]);
        }
    }
}

void Delay::init()
{
    mix->start(startTime);
    delay->start(startTime);
    feedback->start(startTime);
}

LowPassFilter::LowPassFilter(ValueObject* mix, ValueObject* cutoff) :
    mix(mix), cutoff(cutoff) {}

void LowPassFilter::apply(double* buffer, unsigned int bufferLength)
{
    const double omega = tan(utils->pi * cutoff->getValue() / utils->sampleRate);
    const double omega2 = omega * omega;
    const double c = 1 + 2 * cos(utils->pi / 4) * omega + omega2;
    const double a0 = omega2 / c;
    const double a1 = 2 * a0;
    const double b1 = 2 * (omega2 - 1) / c;
    const double b2 = (1 - 2 * cos(utils->pi / 4) * omega + omega2) / c;

    for (unsigned int i = 0; i < bufferLength * utils->channels; i += utils->channels)
    {
        const double mixValue = mix->getValue();

        const double rawl = buffer[i];

        if (utils->channels == 1)
        {
            buffer[i] = mixValue * (a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);

            raw2[0] = raw1[0];
            raw1[0] = rawl;

            filtered2[0] = filtered1[0];
            filtered1[0] = buffer[i];
        }

        else
        {
            const double rawr = buffer[i + 1];

            buffer[i] = mixValue * (a0 * buffer[i] + a1 * raw1[0] + a0 * raw2[0] - b1 * filtered1[0] - b2 * filtered2[0]);
            buffer[i + 1] = mixValue * (a0 * buffer[i + 1] + a1 * raw1[1] + a0 * raw2[1] - b1 * filtered1[1] - b2 * filtered2[1]);

            raw2[0] = raw1[0];
            raw2[1] = raw1[1];
            raw1[0] = rawl;
            raw1[1] = rawr;

            filtered2[0] = filtered1[0];
            filtered2[1] = filtered1[1];
            filtered1[0] = buffer[i];
            filtered1[1] = buffer[i + 1];
        }
    }
}

void LowPassFilter::init()
{
    mix->start(startTime);
    cutoff->start(startTime);
}

Buffer::Buffer(const size_t length) :
    length(length)
{
    buffer = (double*)malloc(sizeof(double) * length);
}

Buffer::~Buffer()
{
    free(buffer);
}

void Buffer::write(const double* source, const size_t length)
{
    if (end >= this->length)
    {
        end = 0;
    }

    memcpy(buffer + end, source, sizeof(double) * length);

    end += length;
}

void Buffer::read(double* dest, const size_t offset, const size_t length) const
{
    memcpy(dest, buffer + offset, sizeof(double) * length);
}

void Buffer::push(const double value)
{
    if (end >= length)
    {
        end = 0;
    }

    buffer[end++] = value;
}

size_t Buffer::size() const
{
    return end;
}

RingBuffer::RingBuffer(const size_t length) :
    length(length)
{
    buffer = (double*)malloc(sizeof(double) * length);
}

RingBuffer::~RingBuffer()
{
    free(buffer);
}

void RingBuffer::push(const double value)
{
    buffer[back++] = value;

    if (back >= length)
    {
        back = 0;
    }

    if (size >= length)
    {
        if (++front >= length)
        {
            front = 0;
        }
    }

    else
    {
        size++;
    }
}

void RingBuffer::add(const size_t offset, const double value)
{
    size_t index = front + offset;

    if (index >= length)
    {
        index -= length;
    }

    buffer[index] += value;
}

double RingBuffer::pop()
{
    if (size == 0)
    {
        return 0;
    }

    const double value = buffer[front++];

    if (front >= length)
    {
        front = 0;
    }

    size--;

    return value;
}

ExecutorThread::ExecutorThread()
{
    thread = std::thread([=]()
    {
        std::unique_lock unique(lock);

        while (executing)
        {
            signal.wait(unique);

            if (executing && !functions.empty())
            {
                while (!functions.empty())
                {
                    functions.front()();
                    functions.pop();
                }

                signal.notify_all();
            }
        }
    });
}

ExecutorThread::~ExecutorThread()
{
    lock.lock();

    executing = false;

    lock.unlock();

    signal.notify_all();

    if (thread.joinable())
    {
        thread.join();
    }
}

void ExecutorThread::schedule(const std::function<void()>& function)
{
    lock.lock();

    functions.push(function);

    lock.unlock();

    signal.notify_all();
}

void ExecutorThread::wait()
{
    std::unique_lock unique(lock);

    while (!functions.empty())
    {
        signal.wait(unique);
    }
}

ExecutorPool::ExecutorPool(const size_t numThreads) :
    numThreads(numThreads)
{
    threads = (ExecutorThread**)malloc(sizeof(ExecutorThread*) * numThreads);

    for (size_t i = 0; i < numThreads; i++)
    {
        threads[i] = new ExecutorThread();
    }
}

ExecutorPool::~ExecutorPool()
{
    free(threads);
}

void ExecutorPool::schedule(const std::function<void()>& function)
{
    threads[next++]->schedule(function);

    if (next >= numThreads)
    {
        next = 0;
    }
}

void ExecutorPool::wait()
{
    for (size_t i = 0; i < numThreads; i++)
    {
        threads[i]->wait();
    }
}

Convolver::Convolver(const size_t length, const size_t offset, const double* impulse) :
    length(length), offset(offset)
{
    powers = (std::complex<double>*)malloc(sizeof(std::complex<double>) * length);

    for (size_t i = 0; i < length; i++)
    {
        powers[i] = std::exp(std::complex(0.0, 2 * M_PI * i / (length * 2)));
    }

    inputBuffer = (double*)calloc(length * 2, sizeof(double));

    buffer1 = (std::complex<double>*)malloc(sizeof(std::complex<double>) * length * 2);
    buffer2 = (std::complex<double>*)malloc(sizeof(std::complex<double>) * length * 2);

    overlap = (double*)calloc(length, sizeof(double));

    this->impulse = (std::complex<double>*)malloc(sizeof(std::complex<double>) * length * 2);

    std::fill(this->impulse, this->impulse + length * 2, 0);

    double* padded = (double*)malloc(sizeof(double) * length * 2);

    std::fill(padded, padded + length * 2, 0);

    memcpy(padded, impulse, sizeof(double) * length);

    fft(padded, length * 2, 1, this->impulse);

    free(padded);
}

Convolver::~Convolver()
{
    free(powers);
    free(inputBuffer);
    free(buffer1);
    free(buffer2);
    free(overlap);
    free(impulse);
}

void Convolver::execute(const Buffer* input, const size_t position, RingBuffer* output)
{
    if (position <= last)
    {
        last = 0;
    }

    if (position >= last + length)
    {
        input->read(inputBuffer, last, length);

        std::fill(buffer1, buffer1 + length * 2, 0);
        std::fill(buffer2, buffer2 + length * 2, 0);

        fft(inputBuffer, length * 2, 1, buffer1);

        for(size_t i = 0; i < length * 2; i++)
        {
            buffer1[i] *= impulse[i];
        }

        ifft(buffer1, length * 2, 1, buffer2);

        for (size_t i = 0; i < length; i++)
        {
            output->add(offset + i, (buffer2[i].real() + overlap[i]) / (length * 2));

            overlap[i] = buffer2[i + length].real();
        }

        last = position;
    }
}

void Convolver::fft(const double* start, const size_t length, const size_t step, std::complex<double>* result) const
{
    if (length == 1)
    {
        result[0] = start[0];

        return;
    }

    fft(start, length / 2, step * 2, result);
    fft(start + step, length / 2, step * 2, result + length / 2);

    for (size_t i = 0; i < length / 2; i++)
    {
        const std::complex<double> even = result[i];
        const std::complex<double> odd = powers[i * step] * result[i + length / 2];

        result[i] += odd;
        result[i + length / 2] = even - odd;
    }
}

void Convolver::ifft(const std::complex<double>* start, const size_t length, const size_t step, std::complex<double>* result) const
{
    if (length == 1)
    {
        result[0] = start[0];

        return;
    }

    ifft(start, length / 2, step * 2, result);
    ifft(start + step, length / 2, step * 2, result + length / 2);

    for (size_t i = 0; i < length / 2; i++)
    {
        const std::complex<double> even = result[i];
        const std::complex<double> odd = result[i + length / 2] / powers[i * step];

        result[i] += odd;
        result[i + length / 2] = even - odd;
    }
}

ConvolverStream::ConvolverStream(const std::vector<size_t>& segments, const double* impulse) :
    segments(segments)
{
    convolvers = (Convolver**)malloc(sizeof(Convolver*) * segments.size());

    size_t length = 0;

    for (size_t i = 0; i < segments.size(); i++)
    {
        convolvers[i] = new Convolver(segments[i], length, impulse + length);

        length += segments[i];
    }

    input = new Buffer(segments.back());

    output = new RingBuffer(length);

    for (size_t i = 0; i < length; i++)
    {
        output->push(0);
    }

    executor = new ExecutorPool(4);
}

ConvolverStream::~ConvolverStream()
{
    free(convolvers);
    free(input);
    free(output);
}

void ConvolverStream::write(const double* source, const size_t length, const size_t stride)
{
    for (size_t i = 0; i < length; i += stride)
    {
        input->push(source[i]);
    }
}

void ConvolverStream::execute()
{
    for (size_t i = 0; i < segments.size(); i++)
    {
        executor->schedule([=]()
        {
            convolvers[i]->execute(input, input->size(), output);
        });
    }

    executor->wait();
}

void ConvolverStream::read(double* dest, const size_t length, const size_t stride, const double mix)
{
    for (size_t i = 0; i < length; i += stride)
    {
        dest[i] = dest[i] * (1 - mix) + output->pop() * mix;

        output->push(0);
    }
}

Reverb::Reverb(ValueObject* mix, ValueObject* response) :
    mix(mix), response(response)
{
    streams = (ConvolverStream**)malloc(sizeof(ConvolverStream*) * utils->channels);

    const Resource* impulse = response->getLeafAs<Resource>();

    size_t segmentSize = 256;
    size_t segmentSum = 0;

    while (segmentSum < impulse->length)
    {
        segments.push_back(segmentSize);

        segmentSum += segmentSize;

        if (segmentSize < 8192)
        {
            segmentSize *= 2;
        }
    }

    double* samples = (double*)malloc(sizeof(double) * segmentSum);

    std::fill(samples, samples + segmentSum, 0);

    memcpy(samples, impulse->samples, sizeof(double) * impulse->length);

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        streams[i] = new ConvolverStream(segments, samples);
    }

    free(samples);
}

Reverb::~Reverb()
{
    free(streams);
}

void Reverb::apply(double* buffer, const unsigned int bufferLength)
{
    for (unsigned int i = 0; i < utils->channels; i++)
    {
        streams[i]->write(buffer + i, bufferLength * utils->channels, utils->channels);
    }

    offset += bufferLength;

    if (offset >= segments[0])
    {
        for (unsigned int i = 0; i < utils->channels; i++)
        {
            streams[i]->execute();
        }

        offset = 0;
    }

    const double mixValue = mix->getValue();

    for (unsigned int i = 0; i < utils->channels; i++)
    {
        streams[i]->read(buffer + i, bufferLength * utils->channels, utils->channels, mixValue);
    }
}

void Reverb::init()
{
    mix->start(startTime);
    response->start(startTime);
}
