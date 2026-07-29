#pragma once

#include <stddef.h>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utils.h"

namespace Engine {

struct Sync
{
    Sync();

    void start(const double time);
    void repeat(const double time);
    void stop(const double time);

    inline double getStopTime() const
    {
        return stopTime;
    }

    bool enabled = false;

protected:
    virtual void init();
    virtual void reinit();

    Utils* utils;

    double startTime = 0;
    double repeatTime = 0;
    double stopTime = 0;

};

struct ValueObject;

struct Defaults
{
    static void deinit();

    template <typename T> static inline T* get()
    {
        const std::type_index index = std::type_index(typeid(T));

        if (!objects.count(index))
        {
            objects[index] = new T();
        }

        return static_cast<T*>(objects[index]);
    }

private:
    static inline std::unordered_map<std::type_index, ValueObject*> objects;

};

struct ValueObject : public Sync
{
    virtual ~ValueObject();

    virtual double getValue() const;

    virtual ValueObject* getLeaf();

    template <typename T> inline T* getLeafAs()
    {
        if (ValueObject* leaf = getLeaf())
        {
            return static_cast<T*>(leaf);
        }

        return Defaults::get<T>();
    }

    virtual void update();
};

struct List : public ValueObject
{
    List(const std::vector<ValueObject*>& objects = {});
    ~List();

    const std::vector<ValueObject*> objects;
};

struct Variable : public ValueObject
{
    Variable(ValueObject* value);

    double getValue() const override;

    ValueObject* getLeaf() override;

    void update() override;

    ValueObject* value;

protected:
    void init() override;

};

struct Lambda : public ValueObject
{
    Lambda(const std::vector<Variable*>& inputs, ValueObject* value);
    Lambda();
    ~Lambda();

    double getValue() const override;

    void update() override;

    void setInputs(const std::vector<ValueObject*>& values);

protected:
    void init() override;

private:
    std::vector<Variable*> inputs;

    ValueObject* value;

};

}
