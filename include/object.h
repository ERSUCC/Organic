#pragma once

#include <vector>

#include "utils.h"

struct Sync
{
    Sync();

    void start(double time);
    void repeat(double time);
    void stop();

    virtual double syncLength() const;

    double startTime = 0;
    double repeatTime = 0;

    bool enabled = false;

protected:
    virtual void init();
    virtual void reinit();

    Utils* utils;

};

struct ValueObject : public Sync
{
    virtual double getValue();

    virtual ValueObject* getLeaf();

    template <typename T> T* getLeafAs()
    {
        return static_cast<T*>(getLeaf());
    }
};

struct List : public ValueObject
{
    List(const std::vector<ValueObject*> objects);

    const std::vector<ValueObject*> objects;
};

struct Variable : public ValueObject
{
    Variable(ValueObject* value);

    double syncLength() const override;
    double getValue() override;

    ValueObject* getLeaf() override;

    ValueObject* value;

protected:
    void init() override;

};

struct Lambda : public ValueObject
{
    Lambda(const std::vector<Variable*> inputs, ValueObject* value);

    double getValue() override;

    void setInputs(const std::vector<ValueObject*>& values);

protected:
    void init() override;

private:
    std::vector<Variable*> inputs;

    ValueObject* value;

};

struct Resource : public ValueObject
{
    Resource(double* samples, const unsigned int length, const unsigned int sampleRate, const unsigned int channels);
    ~Resource();

    double* samples;

    const unsigned int length;
    const unsigned int sampleRate;
    const unsigned int channels;
};

struct Default : public ValueObject
{
    static Default* get();

    explicit operator List*() const;
    explicit operator Lambda*() const;
    explicit operator Resource*() const;

private:
    Default();

    static Default* instance;

    List* list;

    Lambda* lambda;

    Resource* resource;

};

struct Time : public ValueObject
{
    double getValue() override;
};
