#pragma once

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

struct ValueObject : public Sync
{
    virtual double getValue();

    virtual ValueObject* expandVariable();

    virtual List* getList();
};

struct Default : public ValueObject
{
    static Default* get();

    List* getList() override;

private:
    Default();

    static Default* instance;

    List* list;

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

    ValueObject* value;

protected:
    void init() override;

};

struct Time : public ValueObject
{
    double getValue() override;
};
