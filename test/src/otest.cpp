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

TOMLValue::~TOMLValue() {}

const std::vector<const TOMLValue*> TOMLValue::asArray() const
{
    return {};
}

const std::string TOMLValue::asString() const
{
    return "";
}

double TOMLValue::asDouble() const
{
    return 0;
}

int TOMLValue::asInteger() const
{
    return 0;
}

bool TOMLValue::asBoolean() const
{
    return false;
}

TOMLArray* TOMLArray::read(std::istringstream& stream)
{
    stream.ignore();

    while (isspace(stream.peek()))
    {
        stream.ignore();
    }

    TOMLArray* array = new TOMLArray();

    while (!stream.eof() && stream.peek() != ']')
    {
        const TOMLValue* value = TOMLValue::read(stream);

        if (!value)
        {
            delete array;

            return nullptr;
        }

        array->push(value);

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
        delete array;

        return nullptr;
    }

    return array;
}

TOMLArray::~TOMLArray()
{
    for (const TOMLValue* value : values)
    {
        delete value;
    }
}

const std::vector<const TOMLValue*> TOMLArray::asArray() const
{
    return values;
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

const std::string TOMLString::asString() const
{
    return str;
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

double TOMLDouble::asDouble() const
{
    return value;
}

TOMLInteger::TOMLInteger(const int value) :
    value(value) {}

int TOMLInteger::asInteger() const
{
    return value;
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

bool TOMLBoolean::asBoolean() const
{
    return value;
}

std::vector<const OTest*> OTest::read(const Path& path)
{
    std::string str;

    if (!path.readToString(str))
    {
        throw OrganicFileException("Could not read \"" + path.string() + "\".");
    }

    std::vector<const OTest*> tests;

    std::istringstream stream(str);

    while (isspace(stream.peek()))
    {
        stream.ignore();
    }

    while (!stream.eof())
    {
        try
        {
            tests.push_back(new OTest(path, stream));
        }

        catch (const OrganicException& e)
        {
            for (const OTest* info : tests)
            {
                delete info;
            }

            throw;
        }

        while (isspace(stream.peek()))
        {
            stream.ignore();
        }
    }

    return tests;
}

OTest::~OTest()
{
    for (const std::pair<std::string, const TOMLValue*>& entry : entries)
    {
        delete entry.second;
    }
}

const TOMLValue* OTest::getValue(const std::string& key) const
{
    if (entries.count(key))
    {
        return entries.at(key);
    }

    static TOMLValue* dummyValue;

    if (!dummyValue)
    {
        dummyValue = new TOMLValue();
    }

    return dummyValue;
}

const std::string& OTest::getSource() const
{
    return source;
}

bool OTest::keyChar(const char c)
{
    return isalnum(c) || c == '-' || c == '_';
}

OTest::OTest(const Path& path, std::istringstream& stream)
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

        readEntry(path, stream);
    }

    const std::streampos split = stream.tellg();

    while (!stream.eof() && stream.str().substr(stream.tellg(), 3) != "---")
    {
        stream.ignore();
    }

    source = stream.str().substr(split, stream.tellg() - split);

    stream.ignore(4);
}

void OTest::readEntry(const Path& path, std::istringstream& stream)
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
        throw OrganicFileException("Invalid format in test file \"" + path.string() + "\".");
    }

    while (isspace(stream.peek()))
    {
        stream.ignore();
    }

    if (const TOMLValue* value = TOMLValue::read(stream))
    {
        entries[key] = value;
    }

    else
    {
        throw OrganicFileException("Invalid format in test file \"" + path.string() + "\".");
    }
}
