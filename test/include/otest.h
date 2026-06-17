#pragma once

#include <ctype.h>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "exception.h"
#include "path.h"

struct TOMLArray;
struct TOMLString;
struct TOMLDouble;
struct TOMLInteger;
struct TOMLBoolean;

struct TOMLValue
{
    static TOMLValue* read(std::istringstream& stream);

    virtual const TOMLArray* asArray() const;
    virtual const TOMLString* asString() const;
    virtual const TOMLDouble* asDouble() const;
    virtual const TOMLInteger* asInteger() const;
    virtual const TOMLBoolean* asBoolean() const;
};

struct TOMLArray : public TOMLValue
{
    static TOMLArray* read(std::istringstream& stream);

    TOMLArray(const std::vector<TOMLValue*>& values);

    const TOMLArray* asArray() const override;

    const std::vector<TOMLValue*> values;
};

struct TOMLString : public TOMLValue
{
    static TOMLString* read(std::istringstream& stream, const bool escape);

    TOMLString(const std::string& str);

    const TOMLString* asString() const override;

    const std::string str;
};

struct TOMLNumber : public TOMLValue
{
    static TOMLNumber* read(std::istringstream& stream);
};

struct TOMLDouble : public TOMLNumber
{
    TOMLDouble(const double value);

    const TOMLDouble* asDouble() const override;

    const double value;
};

struct TOMLInteger : public TOMLNumber
{
    TOMLInteger(const int value);

    const TOMLInteger* asInteger() const override;

    const int value;
};

struct TOMLBoolean : public TOMLValue
{
    static TOMLBoolean* read(std::istringstream& stream);

    TOMLBoolean(const bool value);

    const TOMLBoolean* asBoolean() const override;

    const bool value;
};

struct TOMLEntry
{
    static TOMLEntry* read(std::istringstream& stream);

    const std::string key;

    const TOMLValue* value;

private:
    static bool keyChar(const char c);

    TOMLEntry(const std::string& key, const TOMLValue* value);

};

struct OTest
{
    static std::vector<OTest*> read(const Path* path);

    const TOMLValue* getValue(const std::string& key) const;

    const std::string& getSource() const;

private:
    OTest(const Path* path, std::istringstream& stream);

    std::unordered_map<std::string, const TOMLValue*> entries;

    std::string source;

};
