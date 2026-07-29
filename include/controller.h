#pragma once

#include <random>
#include <stddef.h>
#include <unordered_set>
#include <vector>

#include "constants.h"
#include "object.h"

namespace Engine {

struct Time : public ValueObject
{
    double getValue() const override;
};

struct Value : public ValueObject
{
    Value(const double value);

    double getValue() const override;

private:
    const double value;

};

struct ValueChar : public ValueObject
{
    ValueChar(const unsigned char value = 0);

    const unsigned char value;
};

struct ValueNegate : public ValueObject
{
    ValueNegate(ValueObject* value);
    ~ValueNegate();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* value;

};

struct ValueCombination : public ValueObject
{
    ValueCombination(ValueObject* value1, ValueObject* value2);
    ~ValueCombination();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

    virtual double getValueInternal(const double value1, const double value2) const = 0;

private:
    ValueObject* value1;
    ValueObject* value2;

};

struct ValueAdd : public ValueCombination
{
    ValueAdd(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueSubtract : public ValueCombination
{
    ValueSubtract(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueMultiply : public ValueCombination
{
    ValueMultiply(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueDivide : public ValueCombination
{
    ValueDivide(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValuePower : public ValueCombination
{
    ValuePower(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueEquals : public ValueCombination
{
    ValueEquals(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueLess : public ValueCombination
{
    ValueLess(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueGreater : public ValueCombination
{
    ValueGreater(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueLessEqual : public ValueCombination
{
    ValueLessEqual(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct ValueGreaterEqual : public ValueCombination
{
    ValueGreaterEqual(ValueObject* value1, ValueObject* value2);

protected:
    double getValueInternal(const double value1, const double value2) const override;

};

struct All : public ValueObject
{
    All(ValueObject* values);
    ~All();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* values;

};

struct Any : public ValueObject
{
    Any(ValueObject* values);
    ~Any();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* values;

};

struct None : public ValueObject
{
    None(ValueObject* values);
    ~None();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* values;

};

struct Min : public ValueObject
{
    Min(ValueObject* values);
    ~Min();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* values;

};

struct Max : public ValueObject
{
    Max(ValueObject* values);
    ~Max();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* values;

};

struct Round : public ValueObject
{
    Round(ValueObject* value, ValueObject* step, ValueObject* direction);
    ~Round();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* value;
    ValueObject* step;
    ValueObject* direction;

};

struct Absolute : public ValueObject
{
    Absolute(ValueObject* value);
    ~Absolute();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* value;

};

struct Sequence : public ValueObject
{
    Sequence(ValueObject* controllers, ValueObject* order);
    ~Sequence();

    double getValue() const override;

    ValueObject* getLeaf() override;

    void update() override;

protected:
    void init() override;
    void reinit() override;

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

    double getValue() const override;

    ValueObject* getLeaf() override;

    void update() override;

protected:
    void init() override;
    void reinit() override;

private:
    ValueObject* value;
    ValueObject* repeats;

    size_t times = 0;

};

struct Hold : public ValueObject
{
    Hold(ValueObject* value, ValueObject* length);
    ~Hold();

    double getValue() const override;

    ValueObject* getLeaf() override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* value;
    ValueObject* length;

};

struct Sweep : public ValueObject
{
    Sweep(ValueObject* from, ValueObject* to, ValueObject* length);
    ~Sweep();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

};

struct LFO : public ValueObject
{
    LFO(ValueObject* from, ValueObject* to, ValueObject* length);
    ~LFO();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* from;
    ValueObject* to;
    ValueObject* length;

};

struct Random : public ValueObject
{
    Random(ValueObject* from, ValueObject* to, ValueObject* length, ValueObject* type);
    ~Random();

    double getValue() const override;

    void update() override;

protected:
    void init() override;

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

    double getValue() const override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* value;
    ValueObject* min;
    ValueObject* max;

};

struct Trigger : public ValueObject
{
    Trigger(ValueObject* condition, ValueObject* value);
    ~Trigger();

    double getValue() const override;

    ValueObject* getLeaf() override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* condition;
    ValueObject* value;

    bool triggered = false;

};

struct If : public ValueObject
{
    If(ValueObject* condition, ValueObject* trueValue, ValueObject* falseValue);
    ~If();

    double getValue() const override;

    ValueObject* getLeaf() override;

    void update() override;

protected:
    void init() override;

private:
    ValueObject* condition;
    ValueObject* trueValue;
    ValueObject* falseValue;

};

}
