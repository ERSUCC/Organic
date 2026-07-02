#pragma once

#include <ctype.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "exception.h"
#include "path.h"

struct TOMLValue
{
    static TOMLValue* read(std::istringstream& stream);

    virtual ~TOMLValue();

    virtual const std::vector<const TOMLValue*> asArray() const;
    virtual const std::string asString() const;
    virtual double asDouble() const;
    virtual int asInteger() const;
    virtual bool asBoolean() const;
};

struct TOMLArray : public TOMLValue
{
    static TOMLArray* read(std::istringstream& stream);

    ~TOMLArray();

    const std::vector<const TOMLValue*> asArray() const override;

    inline void push(const TOMLValue* value)
    {
        values.push_back(value);
    }

private:
    std::vector<const TOMLValue*> values;

};

struct TOMLString : public TOMLValue
{
    static TOMLString* read(std::istringstream& stream, const bool escape);

    TOMLString(const std::string& str);

    const std::string asString() const override;

private:
    const std::string str;

};

struct TOMLNumber : public TOMLValue
{
    static TOMLNumber* read(std::istringstream& stream);
};

struct TOMLDouble : public TOMLNumber
{
    TOMLDouble(const double value);

    double asDouble() const override;

private:
    const double value;

};

struct TOMLInteger : public TOMLNumber
{
    TOMLInteger(const int value);

    int asInteger() const override;

private:
    const int value;

};

struct TOMLBoolean : public TOMLValue
{
    static TOMLBoolean* read(std::istringstream& stream);

    TOMLBoolean(const bool value);

    bool asBoolean() const override;

private:
    const bool value;

};

struct OTest
{
    static std::vector<const OTest*> read(const Path& file);

    ~OTest();

    const TOMLValue* getValue(const std::string& key) const;

    const Path path() const;

    const std::string& getSource() const;

private:
    static bool keyChar(const char c);

    OTest(const Path& file, std::istringstream& stream);

    void readEntry(std::istringstream& stream);

    const Path file;

    std::unordered_map<std::string, const TOMLValue*> entries;

    std::string source;

    const TOMLValue* dummyValue = new TOMLValue();

};
