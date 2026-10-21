#pragma once

#include <random>
#include <stddef.h>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "audiosource.h"
#include "constants.h"
#include "object.h"

namespace Engine {

struct Time : public ValueObject
{
    ValueObject* clone() const override;

    double getValue() const override;
};

struct Value : public ValueObject
{
    Value(const double value);

    ValueObject* clone() const override;

    double getValue() const override;

    const double value;
};

struct ValueChar : public ValueObject
{
    ValueChar(const unsigned char value = 0);

    ValueObject* clone() const override;

    const unsigned char value;
};

struct ValueNegate : public ValueObject
{
    ValueNegate(ValueObject* value);
    ~ValueNegate();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* value;

};

struct ValueCombination : public ValueObject
{
    ValueCombination(ValueObject* value1, ValueObject* value2);
    ~ValueCombination();

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

    virtual double getValueInternal(const double value1, const double value2) const = 0;

    ValueObject* value1;
    ValueObject* value2;

};

struct ValueAdd : public ValueCombination
{
    ValueAdd(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueSubtract : public ValueCombination
{
    ValueSubtract(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueMultiply : public ValueCombination
{
    ValueMultiply(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueDivide : public ValueCombination
{
    ValueDivide(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValuePower : public ValueCombination
{
    ValuePower(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueEquals : public ValueCombination
{
    ValueEquals(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueLess : public ValueCombination
{
    ValueLess(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueGreater : public ValueCombination
{
    ValueGreater(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueLessEqual : public ValueCombination
{
    ValueLessEqual(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueGreaterEqual : public ValueCombination
{
    ValueGreaterEqual(ValueObject* value1, ValueObject* value2);

    ValueObject* clone() const override;

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct All : public ValueObject
{
    All(MultiList* values);
    ~All();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    MultiList* values;

};

struct Any : public ValueObject
{
    Any(MultiList* values);
    ~Any();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    MultiList* values;

};

struct None : public ValueObject
{
    None(MultiList* values);
    ~None();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    MultiList* values;

};

struct Min : public ValueObject
{
    Min(MultiList* values);
    ~Min();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    MultiList* values;

};

struct Max : public ValueObject
{
    Max(MultiList* values);
    ~Max();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    MultiList* values;

};

struct Round : public ValueObject
{
    Round(ValueObject* value, ValueObject* step, ValueObject* direction);
    ~Round();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* value;
    ValueObject* step;
    ValueObject* direction;

};

struct Absolute : public ValueObject
{
    Absolute(ValueObject* value);
    ~Absolute();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* value;

};

struct Modulo : public ValueObject
{
    Modulo(ValueObject* value, ValueObject* divisor);
    ~Modulo();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* value;
    ValueObject* divisor;

};

struct Logarithm : public ValueObject
{
    Logarithm(ValueObject* value, ValueObject* base);
    ~Logarithm();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* value;
    ValueObject* base;

};

struct Sequence : public ValueObject
{
    Sequence(ValueObject* controllers, ValueObject* order);
    ~Sequence();

    ValueObject* clone() const override;

    double getValue() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

protected:
    void init() override;
    void reinit() override;
    void compute() override;

private:
    ValueObject* controllers;
    ValueObject* order;

    std::uniform_int_distribution<size_t> udist;

    std::unordered_set<size_t> chosen;

    size_t current = 0;
    size_t direction = 1;
    size_t last = -1;
    size_t switches = 0;

};

struct Repeat : public ValueObject
{
    Repeat(ValueObject* value, ValueObject* repeats);
    ~Repeat();

    ValueObject* clone() const override;

    double getValue() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

protected:
    void init() override;
    void reinit() override;
    void compute() override;

private:
    ValueObject* value;
    ValueObject* repeats;

    size_t times = 0;

};

struct Hold : public ValueObject
{
    Hold(ValueObject* value, ValueObject* length);
    ~Hold();

    ValueObject* clone() const override;

    double getValue() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* value;
    ValueObject* length;

};

struct Sweep : public ValueObject
{
    Sweep(ValueObject* from, ValueObject* to, ValueObject* length);
    ~Sweep();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

};

struct LFO : public ValueObject
{
    LFO(ValueObject* from, ValueObject* to, ValueObject* length);
    ~LFO();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

};

struct Envelope : public ValueObject
{
    Envelope(ValueObject* from, ValueObject* to, ValueObject* attack, ValueObject* sustain, ValueObject* release);
    ~Envelope();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* attack;
    ValueObject* sustain;
    ValueObject* release;

};

struct Random : public ValueObject
{
    Random(ValueObject* from, ValueObject* to, ValueObject* length, ValueObject* type);
    ~Random();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;
    ValueObject* type;

    double current;
    double next = 0;

    bool first = true;

};

struct Limit : public ValueObject
{
    Limit(ValueObject* value, ValueObject* min, ValueObject* max);
    ~Limit();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* value;
    ValueObject* min;
    ValueObject* max;

};

struct Trigger : public ValueObject
{
    Trigger(ValueObject* condition, ValueObject* value);
    ~Trigger();

    ValueObject* clone() const override;

    double getValue() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* condition;
    ValueObject* value;

    bool triggered = false;

};

struct If : public ValueObject
{
    If(ValueObject* condition, ValueObject* trueValue, ValueObject* falseValue);
    ~If();

    ValueObject* clone() const override;

    double getValue() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* condition;
    ValueObject* trueValue;
    ValueObject* falseValue;

};

struct Amplitude : public ValueObject
{
    Amplitude(ValueObject* source);
    ~Amplitude();

    ValueObject* clone() const override;

    double getValue() const override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* source;

    double* buffer;

};

struct Map : public ValueObject
{
    Map(MultiList* values, Lambda* function);
    ~Map();

    ValueObject* clone() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

protected:
    void init() override;
    void compute() override;

private:
    MultiList* values;

    Lambda* function;

    List* currentList = nullptr;

    std::unordered_map<List*, List*> mapped;

};

}
