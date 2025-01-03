#include "../include/machine.h"

Machine::Machine(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        Utils::fileError("Could not open \"" + path.string() + "\".");
    }

    std::ostringstream stream(std::ios::binary);

    stream << file.rdbuf();

    file.close();

    const std::string& str = stream.str();

    if (str.size() <= BytecodeConstants::HEADER_LENGTH || str.compare(0, BytecodeConstants::OBC_ID_LENGTH, BytecodeConstants::OBC_ID))
    {
        Utils::machineError("Invalid bytecode format.");
    }

    program = std::vector<unsigned char>(str.begin(), str.end());

    variables = (Variable**)calloc(program[BytecodeConstants::OBC_ID_LENGTH], sizeof(Variable*));

    utils = Utils::get();

    start = BytecodeConstants::OBC_ID_LENGTH + 2;

    const unsigned int numResources = program[BytecodeConstants::OBC_ID_LENGTH + 1];

    for (unsigned int i = 0; i < numResources; i++)
    {
        const unsigned int length = readUnsignedInt(start);
        const unsigned int sampleRate = readUnsignedInt(start + 4);

        start += 8;

        std::vector<double> samples;

        double max = 0;

        for (unsigned int j = 0; j < length; j++)
        {
            samples.push_back((double)readInt(start) / INT_MAX);

            if (fabs(samples.back()) > max)
            {
                max = fabs(samples.back());
            }

            start += 4;
        }

        for (unsigned int j = 0; j < length; j++)
        {
            samples[j] /= max;
        }

        resources.push_back(new Resource(samples, sampleRate));
    }
}

Machine::~Machine()
{
    free(variables);
}

void Machine::run()
{
    execute(start, 0);
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

unsigned int Machine::readUnsignedInt(const unsigned int address) const
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

int Machine::readInt(const unsigned int address) const
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

    return *reinterpret_cast<int*>(bytes);
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

void Machine::execute(unsigned int address, const double startTime)
{
    while (true)
    {
        if (address >= program.size())
        {
            return Utils::machineError("Intermediate file is invalid or corrupted, unable to continue execution.");
        }

        switch (program[address])
        {
            case BytecodeConstants::RETURN:
                return;

            case BytecodeConstants::STACK_PUSH_DEFAULT:
                stack.push(Default::get());

                address++;

                break;

            case BytecodeConstants::STACK_PUSH_BYTE:
                stack.push(new Value(program[address + 1]));

                address += 2;

                break;

            case BytecodeConstants::STACK_PUSH_INT:
                stack.push(new Value(readUnsignedInt(address + 1)));

                address += 5;

                break;

            case BytecodeConstants::STACK_PUSH_DOUBLE:
                stack.push(new Value(readDouble(address + 1)));

                address += 9;

                break;

            case BytecodeConstants::STACK_PUSH_ADDRESS:
                stack.push(new Value(readUnsignedInt(address + 1)));

                address += 5;

                break;

            case BytecodeConstants::STACK_PUSH_RESOURCE:
                stack.push(resources[program[address + 1]]);

                address += 2;

                break;

            case BytecodeConstants::SET_VARIABLE:
                variables[program[address + 1]] = new Variable(popStack());

                address += 2;

                break;

            case BytecodeConstants::GET_VARIABLE:
                stack.push(variables[program[address + 1]]);

                address += 2;

                break;

            case BytecodeConstants::CALL_NATIVE:
            {
                std::vector<ValueObject*> inputs;

                for (unsigned char i = 0; i < program[address + 2]; i++)
                {
                    inputs.push_back(popStack());
                }

                switch (program[address + 1])
                {
                    case BytecodeConstants::LIST:
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

                    case BytecodeConstants::TIME:
                        stack.push(new Time());

                        break;

                    case BytecodeConstants::ADD:
                        stack.push(new ValueAdd(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::SUBTRACT:
                        stack.push(new ValueSubtract(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::MULTIPLY:
                        stack.push(new ValueMultiply(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::DIVIDE:
                        stack.push(new ValueDivide(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::POWER:
                        stack.push(new ValuePower(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::EQUAL:
                        stack.push(new ValueEquals(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::LESS:
                        stack.push(new ValueLess(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::GREATER:
                        stack.push(new ValueGreater(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::LESSEQUAL:
                        stack.push(new ValueLessEqual(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::GREATEREQUAL:
                        stack.push(new ValueGreaterEqual(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::SINE:
                        stack.push(new Sine(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case BytecodeConstants::SQUARE:
                        stack.push(new Square(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case BytecodeConstants::TRIANGLE:
                        stack.push(new Triangle(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case BytecodeConstants::SAW:
                        stack.push(new Saw(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case BytecodeConstants::NOISE:
                        stack.push(new Noise(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::SAMPLE:
                        stack.push(new Sample(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case BytecodeConstants::BLEND:
                        stack.push(new Blend(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::HOLD:
                        stack.push(new Hold(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::LFO:
                        stack.push(new LFO(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::SWEEP:
                        stack.push(new Sweep(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::SEQUENCE:
                        stack.push(new Sequence(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::REPEAT:
                        stack.push(new Repeat(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::RANDOM:
                        stack.push(new Random(inputs[0], inputs[1], inputs[2], inputs[3]));

                        break;

                    case BytecodeConstants::LIMIT:
                        stack.push(new Limit(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::TRIGGER:
                        stack.push(new Trigger(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::IF:
                        stack.push(new If(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::DELAY:
                        stack.push(new Delay(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::PLAY:
                    {
                        AudioSource* audioSource = dynamic_cast<AudioSource*>(inputs[0]);

                        audioSources.push_back(audioSource);

                        audioSource->start(startTime);

                        stack.push(audioSource);

                        break;
                    }

                    case BytecodeConstants::PERFORM:
                    {
                        const unsigned int exec = inputs[0]->getValue();

                        Event* event = new Event([=](double startTime)
                        {
                            execute(exec, startTime);
                        }, inputs[1], inputs[2], inputs[3]);

                        events.push_back(event);

                        event->start(startTime);

                        break;
                    }

                    default:
                        return Utils::machineError("Intermediate file is invalid or corrupted, unable to continue execution.");

                }

                address += 3;

                break;
            }

            case BytecodeConstants::CALL_USER:
                execute(readUnsignedInt(address + 1), startTime);

                address += 6;

                break;

            default:
                return Utils::machineError("Intermediate file is invalid or corrupted, unable to continue execution.");

        }
    }
}
