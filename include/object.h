#pragma once

#include <cstdlib>
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

struct List;
struct Lambda;
struct Resource;

struct ValueObject : public Sync
{
    virtual double getValue();

    virtual ValueObject* expandVariable();

    virtual List* getList();

    virtual Lambda* getLambda();

    virtual Resource* getResource();
};

struct Default : public ValueObject
{
    static Default* get();

    List* getList() override;

    Lambda* getLambda() override;

    Resource* getResource() override;

private:
    Default();

    static Default* instance;

    List* list;

    Lambda* lambda;

    Resource* resource;

};

struct List : public ValueObject
{
    List(const std::vector<ValueObject*> objects);

    List* getList() override;

    const std::vector<ValueObject*> objects;
};

struct Variable : public ValueObject
{
    Variable(ValueObject* value);

    double syncLength() const override;
    double getValue() override;

    ValueObject* expandVariable() override;

    List* getList() override;

    Lambda* getLambda() override;

    Resource* getResource() override;

    ValueObject* value;

protected:
    void init() override;

};

struct Lambda : public ValueObject
{
    Lambda(const std::vector<Variable*> inputs, ValueObject* value);

    double getValue() override;

    Lambda* getLambda() override;

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

    Resource* getResource() override;

    double* samples;

    const unsigned int length;
    const unsigned int sampleRate;
    const unsigned int channels;
};

struct Time : public ValueObject
{
    double getValue() override;
};
