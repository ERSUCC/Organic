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

void Machine::execute(unsigned int address, std::vector<ValueObject*>& inputs)
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

                if (variables[id]->enabled)
                {
                    variables[id]->stop();
                    variables[id]->start(utils->time);
                }

                address += 2;

                break;
            }

            case 0x06:
                stack.push(variables[program[address + 1]]);

                address += 2;

                break;

            case 0x07:
            {
                std::vector<ValueObject*> inputs;

                for (unsigned char i = 0; i < program[address + 2]; i++)
                {
                    inputs.push_back(popStack());
                }

                switch (program[address + 1])
                {
                    case 0x00:
                        stack.push(((Variable*)inputs[0])->value);

                        break;

                    case 0x01:
                    {
                        const unsigned int size = inputs[0]->getValue();

                        std::vector<ValueObject*> values;

                        for (unsigned int i = 0; i < size; i++)
                        {
                            values.push_back(popStack());
                        }

                        stack.push(new List<ValueObject>(values));

                        break;
                    }

                    case 0x10:
                        stack.push(new ValueAdd(inputs[0], inputs[1]));

                        break;

                    case 0x11:
                        stack.push(new ValueSubtract(inputs[0], inputs[1]));

                        break;

                    case 0x12:
                        stack.push(new ValueMultiply(inputs[0], inputs[1]));

                        break;

                    case 0x13:
                        stack.push(new ValueDivide(inputs[0], inputs[1]));

                        break;

                    case 0x14:
                        stack.push(new ValuePower(inputs[0], inputs[1]));

                        break;

                    case 0x15:
                        stack.push(new ValueEquals(inputs[0], inputs[1]));

                        break;

                    case 0x16:
                        stack.push(new ValueLess(inputs[0], inputs[1]));

                        break;

                    case 0x17:
                        stack.push(new ValueGreater(inputs[0], inputs[1]));

                        break;

                    case 0x18:
                        stack.push(new ValueLessEqual(inputs[0], inputs[1]));

                        break;

                    case 0x19:
                        stack.push(new ValueGreaterEqual(inputs[0], inputs[1]));

                        break;

                    case 0x30:
                    {
                        Sine* sine = new Sine(inputs[0], inputs[1], inputs[2], inputs[3]);

                        audioSources.push_back(sine);

                        sine->start(utils->time);

                        stack.push(sine);

                        break;
                    }

                    case 0x31:
                    {
                        Square* square = new Square(inputs[0], inputs[1], inputs[2], inputs[3]);

                        audioSources.push_back(square);

                        square->start(utils->time);

                        stack.push(square);

                        break;
                    }

                    case 0x32:
                    {
                        Triangle* triangle = new Triangle(inputs[0], inputs[1], inputs[2], inputs[3]);

                        audioSources.push_back(triangle);

                        triangle->start(utils->time);

                        stack.push(triangle);

                        break;
                    }

                    case 0x33:
                    {
                        Saw* saw = new Saw(inputs[0], inputs[1], inputs[2], inputs[3]);

                        audioSources.push_back(saw);

                        saw->start(utils->time);

                        stack.push(saw);

                        break;
                    }

                    case 0x34:
                    {
                        Noise* noise = new Noise(inputs[0], inputs[1], inputs[2]);

                        audioSources.push_back(noise);

                        noise->start(utils->time);

                        stack.push(noise);

                        break;
                    }

                    case 0x50:
                        stack.push(new Hold(inputs[0], inputs[1]));

                        break;

                    case 0x51:
                        stack.push(new LFO(inputs[0], inputs[1], inputs[2]));

                        break;

                    case 0x52:
                        stack.push(new Sweep(inputs[0], inputs[1], inputs[2]));

                        break;

                    case 0x53:
                        stack.push(new Sequence(dynamic_cast<List<ValueObject>*>(inputs[0]), inputs[1])); // can this be uncasted?

                        break;

                    case 0x54:
                        stack.push(new Repeat(inputs[0], inputs[1]));

                        break;

                    case 0x55:
                        stack.push(new Random(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case 0x56:
                        stack.push(new Limit(inputs[0], inputs[1], inputs[2]));

                        break;

                    case 0x57:
                        stack.push(new Trigger(inputs[0], inputs[1]));

                        break;

                    case 0x58:
                        stack.push(new If(inputs[0], inputs[1], inputs[2]));

                        break;

                    case 0x70:
                        stack.push(new Delay(inputs[0], inputs[1], inputs[2]));

                        break;

                    case 0x90:
                    {
                        const unsigned int exec = inputs[0]->getValue();

                        events.push_back(new Event([=]()
                        {
                            std::vector<ValueObject*> inputs;

                            execute(exec, inputs);
                        }, inputs[1]));

                        break;
                    }

                    default:
                    {
                        std::ostringstream code;

                        code << std::hex << program[address + 1];

                        return Utils::machineError("Unrecognized function code \"" + code.str() + "\"", path);
                    }
                }

                address += 3;

                break;
            }

            case 0x08:
            {
                std::vector<ValueObject*> inputs;

                for (unsigned char i = 0; i < program[address + 5]; i++)
                {
                    inputs.push_back(popStack());
                }

                execute(readInt(program[address + 1]), inputs);

                address += 6;

                break;
            }

            case 0x09:
                inputs[program[address + 1]] = popStack();

                address += 2;

                break;

            case 0x0a:
                stack.push(inputs[program[address + 1]]);

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
