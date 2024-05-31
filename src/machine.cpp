#include "../include/machine.h"

Machine::Machine(const std::string path) : path(path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        Utils::error("Could not open \"" + path + "\".");
    }

    std::ostringstream stream(std::ios::binary);

    stream << file.rdbuf();

    file.close();

    const std::string& str = stream.str();

    program = std::vector<unsigned char>(str.begin(), str.end());

    if (program.size() <= 4 || program[0] != 'B' || program[1] != 'A' || program[2] != 'C' || program[3] != 'H')
    {
        Utils::machineError("Invalid bytecode format.", path);
    }

    utils = Utils::get();
}

void Machine::execute(unsigned int address)
{
    while (true)
    {
        if (address >= program.size())
        {
            return Utils::machineError("Invalid execution address.", path);
        }

        switch (program[address])
        {
            case 0x00:
                return;

            case 0x01:
                stack.push(new Value(program[address + 1])); // probably shouldn't coerce these to double

                address += 2;

                break;

            case 0x02:
                stack.push(new Value(readInt(address + 1))); // probably shouldn't coerce these to double

                address += 5;

                break;

            case 0x03:
                stack.push(new Value(readDouble(address + 1)));

                address += 9;

                break;

            case 0x04:
                stack.push(new Value(readInt(address + 1))); // probably shouldn't coerce these to double

                address += 5;

                break;

            case 0x05:
            {
                const unsigned char id = program[address + 1];

                if (variables.count(id))
                {
                    variables[id]->value = popStack();
                }

                else
                {
                    variables[id] = new Variable(popStack());
                }

                variables[id]->stop();
                variables[id]->start(utils->time);

                address += 2;

                break;
            }

            case 0x06:
                stack.push(variables[program[address + 1]]);

                address += 2;

                break;

            case 0x07:
                stack.push(variables[program[address + 1]]->value);

                address += 2;

                break;

            case 0x08:
                switch (program[address + 1])
                {
                    case 0x00:
                    {
                        const unsigned int size = popStack()->getValue();

                        std::vector<ValueObject*> values;

                        for (unsigned int i = 0; i < size; i++)
                        {
                            values.push_back(popStack());
                        }

                        stack.push(new List<ValueObject>(values));

                        break;
                    }

                    case 0x10:
                        stack.push(new ValueAdd(popStack(), popStack()));

                        break;

                    case 0x11:
                        stack.push(new ValueSubtract(popStack(), popStack()));

                        break;

                    case 0x12:
                        stack.push(new ValueMultiply(popStack(), popStack()));

                        break;

                    case 0x13:
                        stack.push(new ValueDivide(popStack(), popStack()));

                        break;

                    case 0x14:
                        stack.push(new ValuePower(popStack(), popStack()));

                        break;

                    case 0x15:
                        stack.push(new ValueEquals(popStack(), popStack()));

                        break;

                    case 0x16:
                        stack.push(new ValueLess(popStack(), popStack()));

                        break;

                    case 0x17:
                        stack.push(new ValueGreater(popStack(), popStack()));

                        break;

                    case 0x18:
                        stack.push(new ValueLessEqual(popStack(), popStack()));

                        break;

                    case 0x19:
                        stack.push(new ValueGreaterEqual(popStack(), popStack()));

                        break;

                    case 0x30:
                    {
                        Sine* sine = new Sine(popStack(), popStack(), popStack(), popStack());

                        audioSources.push_back(sine);

                        sine->start(utils->time);

                        stack.push(sine);

                        break;
                    }

                    case 0x31:
                    {
                        Square* square = new Square(popStack(), popStack(), popStack(), popStack());

                        audioSources.push_back(square);

                        square->start(utils->time);

                        stack.push(square);

                        break;
                    }

                    case 0x32:
                    {
                        Triangle* triangle = new Triangle(popStack(), popStack(), popStack(), popStack());

                        audioSources.push_back(triangle);

                        triangle->start(utils->time);

                        stack.push(triangle);

                        break;
                    }

                    case 0x33:
                    {
                        Saw* saw = new Saw(popStack(), popStack(), popStack(), popStack());

                        audioSources.push_back(saw);

                        saw->start(utils->time);

                        stack.push(saw);

                        break;
                    }

                    case 0x34:
                    {
                        Noise* noise = new Noise(popStack(), popStack(), popStack());

                        audioSources.push_back(noise);

                        noise->start(utils->time);

                        stack.push(noise);

                        break;
                    }

                    case 0x50:
                        stack.push(new Hold(popStack(), popStack()));

                        break;

                    case 0x51:
                        stack.push(new LFO(popStack(), popStack(), popStack()));

                        break;

                    case 0x52:
                        stack.push(new Sweep(popStack(), popStack(), popStack()));

                        break;

                    case 0x53:
                        stack.push(new Sequence(popStackAsList<ValueObject>(), popStack()));

                        break;

                    case 0x54:
                        stack.push(new Repeat(popStack(), popStack()));

                        break;

                    case 0x55:
                    {
                        stack.push(new Random(popStack(), popStack(), popStack(), popStack()));

                        break;
                    }

                    case 0x56:
                        stack.push(new Limit(popStack(), popStack(), popStack()));

                        break;

                    case 0x57:
                        stack.push(new Trigger(popStack(), popStack()));

                        break;

                    case 0x58:
                        stack.push(new Delay(popStack(), popStack(), popStack()));

                        break;

                    case 0x70:
                    {
                        const unsigned int exec = popStack()->getValue();

                        events.push_back(new Event([=]()
                        {
                            execute(exec);
                        }, popStack()));

                        break;
                    }

                    default:
                    {
                        std::ostringstream code;

                        code << std::hex << program[address + 1];

                        return Utils::machineError("Unrecognized function code \"" + code.str() + "\"", path);
                    }
                }

                address += 2;

                break;

            case 0x09:
                execute(popStack()->getValue());

                inputs.pop();

                address++;

                break;

            case 0x0a:
                inputs.push(std::unordered_map<unsigned char, ValueObject*>());

                address++;

                break;

            case 0x0b:
                inputs.top()[program[address + 1]] = popStack();

                address += 2;

                break;

            case 0x0c:
                stack.push(inputs.top()[program[address + 1]]);

                address += 2;

                break;

            default:
                return Utils::machineError("Unrecognized instruction code.", path);

        }
    }
}

void Machine::processAudioSources(double* buffer, const unsigned int bufferLength)
{
    std::fill(buffer, buffer + bufferLength * utils->channels, 0);

    for (unsigned int i = 0; i < audioSources.size(); i++)
    {
        audioSources[i]->fillBuffer(buffer, bufferLength);
    }
}

void Machine::performEvents()
{
    for (unsigned int i = 0; i < events.size(); i++)
    {
        if (events[i]->ready())
        {
            events[i]->perform();

            if (!events[i]->hasNext())
            {
                events.erase(events.begin() + i);

                i--;
            }
        }
    }
}

unsigned int Machine::readInt(const unsigned int address) const
{
    unsigned char bytes[4];

    if (utils->littleEndian)
    {
        bytes[0] = program[address];
        bytes[1] = program[address + 1];
        bytes[2] = program[address + 2];
        bytes[3] = program[address + 3];
    }

    else
    {
        bytes[0] = program[address + 3];
        bytes[0] = program[address + 2];
        bytes[0] = program[address + 1];
        bytes[0] = program[address];
    }

    return *reinterpret_cast<unsigned int*>(bytes);
}

double Machine::readDouble(const unsigned int address) const
{
    unsigned char bytes[8];

    if (utils->littleEndian)
    {
        bytes[0] = program[address];
        bytes[1] = program[address + 1];
        bytes[2] = program[address + 2];
        bytes[3] = program[address + 3];
        bytes[4] = program[address + 4];
        bytes[5] = program[address + 5];
        bytes[6] = program[address + 6];
        bytes[7] = program[address + 7];
    }

    else
    {
        bytes[1] = program[address + 7];
        bytes[2] = program[address + 6];
        bytes[3] = program[address + 5];
        bytes[4] = program[address + 4];
        bytes[5] = program[address + 3];
        bytes[6] = program[address + 2];
        bytes[7] = program[address + 1];
        bytes[0] = program[address];
    }

    return *reinterpret_cast<double*>(bytes);
}

ValueObject* Machine::popStack()
{
    ValueObject* value = stack.top();

    stack.pop();

    return value;
}

template <typename T> T* Machine::popStackAs()
{
    T* value = dynamic_cast<T*>(stack.top());

    stack.pop();

    return value;
}

template <typename T> List<T>* Machine::popStackAsList()
{
    ValueObject* object = popStack();

    std::vector<T*> objects;

    if (const List<ValueObject>* list = dynamic_cast<const List<ValueObject>*>(object))
    {
        for (ValueObject* object : list->objects)
        {
            objects.push_back(dynamic_cast<T*>(object));
        }
    }

    else if (object)
    {
        objects.push_back(dynamic_cast<T*>(object));
    }

    return new List<T>(objects);
}
