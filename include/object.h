#pragma once

#include "utils.h"

struct Sync
{
    Sync();

    void start(double time);
    void repeat(double time);
    void stop();

    virtual double syncLength();

    double startTime = 0;
    double repeatTime = 0;

    bool enabled = false;

protected:
    virtual void init();
    virtual void reinit();

    Utils* utils;

};

template <typename T> struct List;

struct ValueObject : public Sync
{
    virtual double getValue();

    virtual List<ValueObject>* getList();
};

template <typename T> struct List : public ValueObject
{
    List(const std::vector<T*> objects);

    List<ValueObject>* getList() override;

    const std::vector<T*> objects;
};

struct Variable : public ValueObject
{
    Variable(ValueObject* value);

    double syncLength() override;
    double getValue() override;

    List<ValueObject>* getList() override;

    ValueObject* value;

protected:
    void init() override;

};
