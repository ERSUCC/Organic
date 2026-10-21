#pragma once

#include <stddef.h>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "utils.h"

namespace Engine {

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

struct ValueObject
{
    ValueObject();

    virtual ~ValueObject();

    virtual ValueObject* clone() const;

    template <typename T> inline T* cloneAs()
    {
        return static_cast<T*>(clone());
    }

    void start(const double time);
    void repeat(const double time);
    void stop(const double time);
    void update();

    inline double getStartTime() const
    {
        return startTime;
    }

    inline double getStopTime() const
    {
        return stopTime;
    }

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

    virtual void getLeaves(std::unordered_set<ValueObject*>& leaves);

    bool enabled = false;

protected:
    virtual void init();
    virtual void reinit();
    virtual void compute();

    Utils* utils;

    double startTime = 0;
    double repeatTime = 0;
    double stopTime = 0;

private:
    double lastUpdate = -1;

};

struct String : public ValueObject
{
    String(const std::string& value);
    String();

    ValueObject* clone() const override;

    const std::string value;
};

struct List : public ValueObject
{
    List(const std::vector<ValueObject*>& objects = {});
    ~List();

    ValueObject* clone() const override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

    const std::vector<ValueObject*> objects;
};

struct MultiList : public ValueObject
{
    MultiList(ValueObject* object);
    ~MultiList();

    ValueObject* clone() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* object;

    List* currentList = nullptr;

};

struct Variable : public ValueObject
{
    Variable(ValueObject* value, const bool unique);

    ValueObject* clone() const override;

    double getValue() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* value;

    const bool unique;

};

struct InputMap;

struct Fillable : public ValueObject
{
    Fillable(InputMap* inputs, const std::string& input, ValueObject* value);

    ValueObject* clone() const override;

    double getValue() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

    ValueObject* value;

protected:
    void init() override;
    void compute() override;

private:
    InputMap* inputs;

    const std::string input;

};

struct InputMap
{
    InputMap(const std::unordered_map<std::string, Engine::Fillable*>& inputs = {});

    InputMap* clone();

    inline bool contains(const std::string& input) const
    {
        return inputs.count(input);
    }

    inline Fillable* getInput(const std::string& input) const
    {
        return inputs.at(input);
    }

    Fillable* addInput(const std::string& input);
    Fillable* cloneInput(const std::string& input, ValueObject* value);

    void setInput(const std::string& input, ValueObject* value);

private:
    std::unordered_map<std::string, Engine::Fillable*> inputs;
    std::unordered_map<std::string, Engine::Fillable*> cloned;

};

struct Lambda : public ValueObject
{
    Lambda(InputMap* inputs, ValueObject* value);
    ~Lambda();

    ValueObject* clone() const override;

    double getValue() const override;

    ValueObject* getLeaf() override;

    void getLeaves(std::unordered_set<ValueObject*>& leaves) override;

    void setInput(const std::string& name, ValueObject* value);

protected:
    void init() override;
    void compute() override;

private:
    InputMap* inputs;

    ValueObject* value;

};

}
