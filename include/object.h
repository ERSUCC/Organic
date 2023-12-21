#pragma once

#include "utils.h"

struct Object
{
    virtual ~Object();
};

struct Sync : public Object
{
    Sync();

    void start();
    void start(double time);
    void stop();

    virtual double syncLength();

    Sync* parent = nullptr;

    double startTime = 0;

    bool enabled = false;

protected:
    virtual void finishStart();
    virtual void finishStop();

    virtual void childStart(Sync* child);
    virtual void childStop(Sync* child);

    Utils* utils;

};

struct ValueObject : public Sync
{
    double getValue();

protected:
    virtual double getValueUnchecked() = 0;

};

struct Variable : public ValueObject
{
    double syncLength() override;

    ValueObject* value;

protected:
    void finishStart() override;
    void finishStop() override;

    void childStart(Sync* child) override;
    void childStop(Sync* child) override;

    double getValueUnchecked() override;

};

struct ValueCombination : public ValueObject
{
    double syncLength() override;

    ValueObject* value1;
    ValueObject* value2;

protected:
    void finishStart() override;
    void finishStop() override;

};

struct ValueAdd : public ValueCombination
{

protected:
    double getValueUnchecked() override;

};

struct ValueSubtract : public ValueCombination
{

protected:
    double getValueUnchecked() override;

};

struct ValueMultiply : public ValueCombination
{

protected:
    double getValueUnchecked() override;

};

struct ValueDivide : public ValueCombination
{

protected:
    double getValueUnchecked() override;

};
