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

void Machine::execute(unsigned int address, std::vector<ValueObject*>& inputs, const double startTime)
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
                stack.push(Default::get());

                address++;

                break;

            case 0x02:
                stack.push(new Value(program[address + 1]));

                address += 2;

                break;

            case 0x03:
                stack.push(new Value(readInt(address + 1)));

                address += 5;

                break;

            case 0x04:
                stack.push(new Value(readDouble(address + 1)));

                address += 9;

                break;

            case 0x05:
                stack.push(new Value(readInt(address + 1)));

                address += 5;

                break;

            case 0x06:
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

            case 0x07:
                stack.push(variables[program[address + 1]]);

                address += 2;

                break;

            case 0x08:
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

                        stack.push(new List(values));

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
                        stack.push(new Sine(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case 0x31:
                        stack.push(new Square(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case 0x32:
                        stack.push(new Triangle(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case 0x33:
                        stack.push(new Saw(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case 0x34:
                        stack.push(new Noise(inputs[0], inputs[1], inputs[2]));

                        break;

                    case 0x35:
                        stack.push(new Blend(inputs[0], inputs[1]));

                        break;

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
                        stack.push(new Sequence(inputs[0], inputs[1]));

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
                        AudioSource* audioSource = dynamic_cast<AudioSource*>(inputs[0]);

                        audioSources.push_back(audioSource);

                        audioSource->start(startTime);

                        break;
                    }

                    case 0x91:
                    {
                        const unsigned int exec = inputs[0]->getValue();

                        Event* event = new Event([=](double startTime)
                        {
                            std::vector<ValueObject*> inputs;

                            execute(exec, inputs, startTime);
                        }, inputs[1], inputs[2], inputs[3]);

                        events.push_back(event);

                        event->start(startTime);

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

            case 0x09:
            {
                std::vector<ValueObject*> inputs;

                for (unsigned char i = 0; i < program[address + 5]; i++)
                {
                    inputs.push_back(popStack());
                }

                execute(readInt(address + 1), inputs, startTime);

                address += 6;

                break;
            }

            case 0x0a:
                inputs[program[address + 1]] = popStack();

                address += 2;

                break;

            case 0x0b:
                stack.push(inputs[program[address + 1]]);

                address += 2;

                break;

            default:
                return Utils::machineError("Unrecognized instruction code.", path);

        }
    }
}

void Machine::updateEvents()
{
    for (unsigned int i = 0; i < events.size(); i++)
    {
        events[i]->update();

        if (!events[i]->enabled)
        {
            events.erase(events.begin() + i--);
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

    for (unsigned int i = 0; i < bufferLength * utils->channels; i++)
    {
        buffer[i] *= utils->volume;
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
        bytes[1] = program[address + 2];
        bytes[2] = program[address + 1];
        bytes[3] = program[address];
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
        bytes[0] = program[address + 7];
        bytes[1] = program[address + 6];
        bytes[2] = program[address + 5];
        bytes[3] = program[address + 4];
        bytes[4] = program[address + 3];
        bytes[5] = program[address + 2];
        bytes[6] = program[address + 1];
        bytes[7] = program[address];
    }

    return *reinterpret_cast<double*>(bytes);
}

ValueObject* Machine::popStack()
{
    ValueObject* value = stack.top();

    stack.pop();

    return value;
}
