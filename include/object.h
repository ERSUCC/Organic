#pragma once

#include <stddef.h>
#include <vector>

#include "utils.h"

namespace Engine {

struct Sync
{
    Sync();

    void start(double time);
    void repeat(double time);
    void stop();

    virtual double syncLength() const;

    bool enabled = false;

protected:
    virtual void init();
    virtual void reinit();

    Utils* utils;

    double startTime = 0;
    double repeatTime = 0;

};

struct ValueObject : public Sync
{
    virtual double getValue();

    virtual ValueObject* getLeaf();

    template <typename T> inline T* getLeafAs()
    {
        return static_cast<T*>(getLeaf());
    }
};

struct List : public ValueObject
{
    List(const std::vector<ValueObject*> objects);

    const std::vector<ValueObject*> objects;

protected:
    void init() override;

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

struct Time : public ValueObject
{
    double getValue() override;
};

}
