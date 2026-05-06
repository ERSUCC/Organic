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

    for (unsigned int i = 0; i < str[BytecodeConstants::OBC_ID_LENGTH]; i++)
    {
        variables[i] = new Variable(nullptr);
    }

    const unsigned int numResources = str[BytecodeConstants::OBC_ID_LENGTH + 1];

    resources = (ValueObject**)malloc(sizeof(ValueObject*) * numResources);

    const unsigned int programOffset = readUnsignedInt(str.c_str() + BytecodeConstants::OBC_ID_LENGTH + 2);

    programLength = str.size() - programOffset;

    const char* chars = str.c_str() + BytecodeConstants::HEADER_LENGTH;

    for (unsigned int i = 0; i < numResources; i++)
    {
        const unsigned int length = readUnsignedInt(chars);
        const unsigned int sampleRate = readUnsignedInt(chars + 4);
        const unsigned int channels = readUnsignedInt(chars + 8);

        chars += 12;

        float* samples = (float*)malloc(sizeof(float) * length);

        float max = 0;

        for (unsigned int j = 0; j < length; j++)
        {
            samples[j] = (float)readInt(chars + j * 4) / INT_MAX;

            if (fabs(samples[j]) > max)
            {
                max = fabs(samples[j]);
            }
        }

        if (max != 0)
        {
            for (unsigned int j = 0; j < length; j++)
            {
                samples[j] /= max;
            }
        }

        unsigned int scaledLength = length;

        if (sampleRate != utils->sampleRate)
        {
            scaledLength = length * (double)utils->sampleRate / sampleRate;

            float* scaledSamples = (float*)malloc(sizeof(float) * scaledLength);

            SRC_DATA data;

            data.data_in = samples;
            data.data_out = scaledSamples;
            data.input_frames = length / channels;
            data.output_frames = scaledLength / channels;
            data.src_ratio = (double)utils->sampleRate / sampleRate;

            const int result = src_simple(&data, SRC_SINC_BEST_QUALITY, channels);

            if (result)
            {
                throw OrganicMachineException(std::string("Failed to convert sample rate of audio file: ") + src_strerror(result));
            }

            free(samples);

            samples = scaledSamples;
            scaledLength = data.output_frames_gen * utils->channels;
        }

        double* doubleSamples = (double*)calloc(scaledLength, sizeof(double));

        if (channels == utils->channels)
        {
            for (size_t j = 0; j < scaledLength; j++)
            {
                doubleSamples[j] = samples[j];
            }
        }

        else if (channels == 1)
        {
            for (size_t j = 0; j < scaledLength; j++)
            {
                doubleSamples[j] = samples[j / 2] / 2;
            }
        }

        else
        {
            for (size_t j = 0; j < scaledLength; j++)
            {
                doubleSamples[j / 2] += samples[j];
            }
        }

        free(samples);

        resources[i] = new Resource(doubleSamples, scaledLength);

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
    while (!stack.empty())
    {
        stack.pop();
    }

    execute(0, 0);

    while (!stack.empty())
    {
        if (AudioSource* audioSource = dynamic_cast<AudioSource*>(stack.top()))
        {
            audioSources.push_back(audioSource);

            audioSource->start(0);
        }

        stack.pop();
    }
}

void Machine::processAudioSources(double* buffer)
{
    memset(buffer, 0, sizeof(double) * utils->channels);

    for (unsigned int i = 0; i < audioSources.size(); i++)
    {
        audioSources[i]->fillBuffer(buffer);
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

            case BytecodeConstants::STACK_PUSH_BYTE:
                stack.push(new ValueByte(program[address + 1]));

                address += 2;

                break;

            case BytecodeConstants::STACK_PUSH_INT:
                stack.push(new ValueInt(readUnsignedInt(program + address + 1)));

                address += 5;

                break;

            case BytecodeConstants::STACK_PUSH_DOUBLE:
                stack.push(new Value(readDouble(program + address + 1)));

                address += 9;

                break;

            case BytecodeConstants::STACK_PUSH_ADDRESS:
                stack.push(new ValueInt(readUnsignedInt(program + address + 1)));

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
                        const unsigned int size = static_cast<ValueInt*>(inputs[0])->value;

                        std::vector<ValueObject*> values;

                        for (unsigned int i = 0; i < size; i++)
                        {
                            values.push_back(popStack());
                        }

                        stack.push(new List(values));

                        break;
                    }

                    case BytecodeConstants::LAMBDA:
                    {
                        const unsigned int size = static_cast<ValueInt*>(inputs[0])->value;

                        std::vector<Variable*> placeholders;

                        for (unsigned int i = 0; i < size; i++)
                        {
                            placeholders.push_back(static_cast<Variable*>(popStack()));
                        }

                        execute(static_cast<ValueInt*>(inputs[1])->value, startTime);

                        stack.push(new Lambda(placeholders, popStack()));

                        break;
                    }

                    case BytecodeConstants::EMPTY_LAMBDA:
                        stack.push(new Lambda({}, inputs[0]));

                        break;

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

                    case BytecodeConstants::LESS_EQUAL:
                        stack.push(new ValueLessEqual(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::GREATER_EQUAL:
                        stack.push(new ValueGreaterEqual(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::ALL:
                        stack.push(new All(inputs[0]));

                        break;

                    case BytecodeConstants::ANY:
                        stack.push(new Any(inputs[0]));

                        break;

                    case BytecodeConstants::NONE:
                        stack.push(new None(inputs[0]));

                        break;

                    case BytecodeConstants::MIN:
                        stack.push(new Min(inputs[0]));

                        break;

                    case BytecodeConstants::MAX:
                        stack.push(new Max(inputs[0]));

                        break;

                    case BytecodeConstants::ROUND:
                        stack.push(new Round(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::ABS:
                        stack.push(new Absolute(inputs[0]));

                        break;

                    case BytecodeConstants::EMPTY_AUDIO_SOURCE:
                        stack.push(new AudioSource());

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

                    case BytecodeConstants::OSCILLATOR:
                        stack.push(new CustomOscillator(inputs[0], inputs[1], inputs[2], inputs[3], inputs[4]));

                        break;

                    case BytecodeConstants::GRANULATE:
                        stack.push(new Granulate(inputs[0], inputs[1], inputs[2], inputs[3], inputs[4], inputs[5], inputs[6]));

                        break;

                    case BytecodeConstants::GROUP:
                        stack.push(new Group(inputs[0], inputs[1], inputs[2], inputs[3]));

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

                    case BytecodeConstants::EMPTY_EFFECT:
                        stack.push(new Effect());

                        break;

                    case BytecodeConstants::EFFECT_GROUP:
                        stack.push(new EffectGroup(inputs[0], inputs[1]));

                        break;

                    case BytecodeConstants::DELAY:
                        stack.push(new Delay(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::COMB:
                        stack.push(new Comb(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::ALL_PASS:
                        stack.push(new AllPass(inputs[0], inputs[1], inputs[2]));

                        break;

                    case BytecodeConstants::LOW_PASS:
                        stack.push(new LowPass(inputs[0]));

                        break;

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
