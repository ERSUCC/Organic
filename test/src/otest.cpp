#include "../include/otest.h"

TOMLValue* TOMLValue::read(std::istringstream& stream)
{
    const char c = stream.peek();

    switch (c)
    {
        case '[':
            return TOMLArray::read(stream);

        case '"':
            return TOMLString::read(stream, true);

        case '\'':
            return TOMLString::read(stream, false);

        default:
            if (isdigit(c) || c == '+' || c == '-')
            {
                return TOMLNumber::read(stream);
            }

            return TOMLBoolean::read(stream);
    }
}

const TOMLArray* TOMLValue::asArray() const
{
    return new TOMLArray({});
}

const TOMLString* TOMLValue::asString() const
{
    return new TOMLString("");
}

const TOMLDouble* TOMLValue::asDouble() const
{
    return new TOMLDouble(0);
}

const TOMLInteger* TOMLValue::asInteger() const
{
    return new TOMLInteger(0);
}

const TOMLBoolean* TOMLValue::asBoolean() const
{
    return new TOMLBoolean(false);
}

TOMLArray* TOMLArray::read(std::istringstream& stream)
{
    stream.ignore();

    while (isspace(stream.peek()))
    {
        stream.ignore();
    }

    std::vector<TOMLValue*> values;

    while (!stream.eof() && stream.peek() != ']')
    {
        TOMLValue* value = TOMLValue::read(stream);

        if (!value)
        {
            return nullptr;
        }

        values.push_back(value);

        while (isspace(stream.peek()))
        {
            stream.ignore();
        }

        const char c = stream.peek();

        if (c == ',')
        {
            stream.ignore();

            while (isspace(stream.peek()))
            {
                stream.ignore();
            }
        }

        else if (c != ']')
        {
            break;
        }
    }

    if (stream.get() != ']')
    {
        return nullptr;
    }

    return new TOMLArray(values);
}

TOMLArray::TOMLArray(const std::vector<TOMLValue*>& values) :
    values(values) {}

const TOMLArray* TOMLArray::asArray() const
{
    return this;
}

TOMLString* TOMLString::read(std::istringstream& stream, const bool escape)
{
    stream.ignore();

    std::string str;

    if (escape)
    {
        char last = '\0';

        while (!stream.eof() && (stream.peek() != '"' || last == '\\'))
        {
            last = stream.get();

            if (last != '\\')
            {
                str += last;
            }
        }

        if (stream.get() != '"')
        {
            return nullptr;
        }
    }

    else
    {
        while (!stream.eof() && stream.peek() != '\'')
        {
            str += stream.get();
        }

        if (stream.get() != '\'')
        {
            return nullptr;
        }
    }

    return new TOMLString(str);
}

TOMLString::TOMLString(const std::string& str) :
    str(str) {}

const TOMLString* TOMLString::asString() const
{
    return this;
}

TOMLNumber* TOMLNumber::read(std::istringstream& stream)
{
    std::string str;

    const char c = stream.peek();

    if (c == '+' || c == '-')
    {
        str += stream.get();
    }

    while (isdigit(stream.peek()))
    {
        str += stream.get();
    }

    if (stream.peek() == '.')
    {
        str += stream.get();

        if (!isdigit(stream.peek()))
        {
            return nullptr;
        }

        while (isdigit(stream.peek()))
        {
            str += stream.get();
        }

        return new TOMLDouble(std::stod(str));
    }

    return new TOMLInteger(std::stoi(str));
}

TOMLDouble::TOMLDouble(const double value) :
    value(value) {}

const TOMLDouble* TOMLDouble::asDouble() const
{
    return this;
}

TOMLInteger::TOMLInteger(const int value) :
    value(value) {}

const TOMLInteger* TOMLInteger::asInteger() const
{
    return this;
}

TOMLBoolean* TOMLBoolean::read(std::istringstream& stream)
{
    std::string str;

    while (isalpha(stream.peek()))
    {
        str += stream.get();
    }

    if (str == "true")
    {
        return new TOMLBoolean(true);
    }

    if (str == "false")
    {
        return new TOMLBoolean(false);
    }

    return nullptr;
}

TOMLBoolean::TOMLBoolean(const bool value) :
    value(value) {}

const TOMLBoolean* TOMLBoolean::asBoolean() const
{
    return this;
}

TOMLEntry* TOMLEntry::read(std::istringstream& stream)
{
    std::string key;

    while (keyChar(stream.peek()))
    {
        key += stream.get();
    }

    while (isspace(stream.peek()))
    {
        stream.ignore();
    }

    if (stream.get() != '=')
    {
        return nullptr;
    }

    while (isspace(stream.peek()))
    {
        stream.ignore();
    }

    if (const TOMLValue* value = TOMLValue::read(stream))
    {
        return new TOMLEntry(key, value);
    }

    return nullptr;
}

bool TOMLEntry::keyChar(const char c)
{
    return isalnum(c) || c == '-' || c == '_';
}

TOMLEntry::TOMLEntry(const std::string& key, const TOMLValue* value) :
    key(key), value(value) {}

std::vector<OTest*> OTest::read(const Path* path)
{
    std::string str;

    if (!path->readToString(str))
    {
        throw OrganicFileException("Could not read \"" + path->string() + "\".");
    }

    std::vector<OTest*> tests;

    std::istringstream stream(str);

    while (isspace(stream.peek()))
    {
        stream.ignore();
    }

    while (!stream.eof())
    {
        tests.push_back(new OTest(path, stream));

        while (isspace(stream.peek()))
        {
            stream.ignore();
        }
    }

    return tests;
}

const TOMLValue* OTest::getValue(const std::string& key) const
{
    return entries.at(key);
}

const std::string& OTest::getSource() const
{
    return source;
}

OTest::OTest(const Path* path, std::istringstream& stream)
{
    while (!stream.eof())
    {
        while (isspace(stream.peek()))
        {
            stream.ignore();
        }

        if (stream.str().substr(stream.tellg(), 3) == "---")
        {
            stream.ignore(4);

            break;
        }

        if (const TOMLEntry* entry = TOMLEntry::read(stream))
        {
            entries[entry->key] = entry->value;
        }

        else
        {
            throw OrganicFileException("Invalid format in test file \"" + path->string() + "\".");
        }
    }

    const std::streampos split = stream.tellg();

    while (!stream.eof() && stream.str().substr(stream.tellg(), 3) != "---")
    {
        stream.ignore();
    }

    source = stream.str().substr(split, stream.tellg() - split);

    stream.ignore(4);
}
