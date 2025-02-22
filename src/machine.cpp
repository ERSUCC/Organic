#include "../include/machine.h"

Machine::Machine(const Path* path)
{
    std::string str;

    if (!path->readToStringBinary(str))
    {
        throw OrganicFileException("Could not open \"" + path->string() + "\".");
    }

    if (str.size() <= BytecodeConstants::HEADER_LENGTH || str.compare(0, BytecodeConstants::OBC_ID_LENGTH, BytecodeConstants::OBC_ID))
    {
        throw OrganicMachineException("Invalid bytecode format.");
    }

    utils = Utils::get();

    variables = (Variable**)malloc(sizeof(Variable*) * str[BytecodeConstants::OBC_ID_LENGTH]);

    const unsigned int numResources = str[BytecodeConstants::OBC_ID_LENGTH + 1];

    resources = (ValueObject**)malloc(sizeof(ValueObject*) * numResources);

    const unsigned int programOffset = readUnsignedInt(str.c_str() + BytecodeConstants::OBC_ID_LENGTH + 2);

    programLength = str.size() - programOffset;

    const char* chars = str.c_str() + BytecodeConstants::HEADER_LENGTH;

    for (unsigned int i = 0; i < numResources; i++)
    {
        const unsigned int length = readUnsignedInt(chars + i * 12);
        const unsigned int sampleRate = readUnsignedInt(chars + i * 12 + 4);
        const unsigned int channels = readUnsignedInt(chars + i * 12 + 8);

        chars += 12;

        double* samples = (double*)malloc(sizeof(double) * length);

        double max = 0;

        for (unsigned int j = 0; j < length; j++)
        {
            samples[j] = (double)readInt(chars + j * 4) / INT_MAX;

            if (fabs(samples[j]) > max)
            {
                max = fabs(samples[j]);
            }
        }

        for (unsigned int j = 0; j < length; j++)
        {
            samples[j] /= max;
        }

        resources[i] = new Resource(samples, length, sampleRate, channels);

        chars += length * 4;
    }

    program = (unsigned char*)malloc(sizeof(unsigned char) * programLength);

    std::copy(chars, chars + programLength, program);
}

Machine::~Machine()
{
    free(program);
    free(resources);
    free(variables);
}

void Machine::run()
{
    execute(0, 0);
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

unsigned int Machine::readUnsignedInt(const void* buffer) const
{
    if (utils->littleEndian)
    {
        return *reinterpret_cast<const unsigned int*>(buffer);
    }

    const unsigned char* chars = reinterpret_cast<const unsigned char*>(buffer);

    unsigned char bytes[4];

    bytes[0] = chars[3];
    bytes[1] = chars[2];
    bytes[2] = chars[1];
    bytes[3] = chars[0];

    return *reinterpret_cast<unsigned int*>(bytes);
}

int Machine::readInt(const void* buffer) const
{
    if (utils->littleEndian)
    {
        return *reinterpret_cast<const unsigned int*>(buffer);
    }

    const unsigned char* chars = reinterpret_cast<const unsigned char*>(buffer);

    unsigned char bytes[4];

    bytes[0] = chars[3];
    bytes[1] = chars[2];
    bytes[2] = chars[1];
    bytes[3] = chars[0];

    return *reinterpret_cast<int*>(bytes);
}

double Machine::readDouble(const void* buffer) const
{
    if (utils->littleEndian)
    {
        return *reinterpret_cast<const double*>(buffer);
    }

    const unsigned char* chars = reinterpret_cast<const unsigned char*>(buffer);

    unsigned char bytes[8];

    bytes[0] = chars[7];
    bytes[1] = chars[6];
    bytes[2] = chars[5];
    bytes[3] = chars[4];
    bytes[4] = chars[3];
    bytes[5] = chars[2];
    bytes[6] = chars[1];
    bytes[7] = chars[0];

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
        if (address >= programLength)
        {
            throw OrganicMachineException("Intermediate file is invalid or corrupted, unable to continue execution.");
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
                stack.push(new Value(readUnsignedInt(program + address + 1)));

                address += 5;

                break;

            case BytecodeConstants::STACK_PUSH_DOUBLE:
                stack.push(new Value(readDouble(program + address + 1)));

                address += 9;

                break;

            case BytecodeConstants::STACK_PUSH_ADDRESS:
                stack.push(new Value(readUnsignedInt(program + address + 1)));

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
                        throw OrganicMachineException("Intermediate file is invalid or corrupted, unable to continue execution.");

                }

                address += 3;

                break;
            }

            case BytecodeConstants::CALL_USER:
                execute(readUnsignedInt(program + address + 1), startTime);

                address += 6;

                break;

            default:
                throw OrganicMachineException("Intermediate file is invalid or corrupted, unable to continue execution.");

        }
    }
}
