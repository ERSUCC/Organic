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
            {
                const unsigned char id = program[address + 1];

                if (variables.count(id))
                {
                    variables[id]->value = popStackAs<ValueObject>();
                }

                else
                {
                    variables[id] = new Variable(popStackAs<ValueObject>());
                }

                address += 2;

                break;
            }

            case 0x05:
                stack.push(variables[program[address + 1]]);

                address += 2;

                break;

            case 0x06:
                stack.push(new Variable(variables[program[address + 1]]->value));

                address += 2;

                break;

            case 0x07:
                switch (program[address + 1])
                {
                    case 0x00:
                    {
                        unsigned int size = popStackAs<Value>()->value;

                        std::vector<Object*> values;

                        for (unsigned int i = 0; i < size; i++)
                        {
                            values.push_back(popStack());
                        }

                        stack.push(new List<Object>(values));

                        address += 2;

                        break;
                    }

                    case 0x01:
                        stack.push(new ValueAdd(popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x02:
                        stack.push(new ValueSubtract(popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x03:
                        stack.push(new ValueMultiply(popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x04:
                        stack.push(new ValueDivide(popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x05:
                    {
                        Sine* sine = new Sine(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAsList<Effect>(), popStackAs<ValueObject>());

                        audioSources.push_back(sine);

                        sine->start(utils->time);

                        stack.push(sine);

                        address += 2;

                        break;
                    }

                    case 0x06:
                    {
                        Square* square = new Square(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAsList<Effect>(), popStackAs<ValueObject>());

                        audioSources.push_back(square);

                        square->start(utils->time);

                        stack.push(square);

                        address += 2;

                        break;
                    }

                    case 0x07:
                    {
                        Triangle* triangle = new Triangle(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAsList<Effect>(), popStackAs<ValueObject>());

                        audioSources.push_back(triangle);

                        triangle->start(utils->time);

                        stack.push(triangle);

                        address += 2;

                        break;
                    }

                    case 0x08:
                    {
                        Saw* saw = new Saw(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAsList<Effect>(), popStackAs<ValueObject>());

                        audioSources.push_back(saw);

                        saw->start(utils->time);

                        stack.push(saw);

                        address += 2;

                        break;
                    }

                    case 0x09:
                    {
                        Noise* noise = new Noise(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAsList<Effect>());

                        audioSources.push_back(noise);

                        noise->start(utils->time);

                        stack.push(noise);

                        address += 2;

                        break;
                    }

                    case 0x0a:
                        stack.push(new Hold(popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x0b:
                        stack.push(new LFO(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x0c:
                        stack.push(new Sweep(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x0d:
                    {
                        List<ValueObject>* list = popStackAsList<ValueObject>();

                        Sequence::Order* order;

                        switch ((unsigned char)popStackAs<Value>()->value)
                        {
                            case 0x00:
                                order = new Sequence::Order(Sequence::OrderEnum::Forwards);

                                break;

                            case 0x01:
                                order = new Sequence::Order(Sequence::OrderEnum::Backwards);

                                break;

                            case 0x02:
                                order = new Sequence::Order(Sequence::OrderEnum::PingPong);

                                break;

                            case 0x03:
                                order = new Sequence::Order(Sequence::OrderEnum::Random);

                                break;

                            default:
                                return Utils::machineError("Unrecognized constant code.", path);
                        }

                        stack.push(new Sequence(list, order));

                        address += 2;

                        break;
                    }

                    case 0x0e:
                        stack.push(new Repeat(popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x0f:
                    {
                        ValueObject* from = popStackAs<ValueObject>();
                        ValueObject* to = popStackAs<ValueObject>();
                        ValueObject* length = popStackAs<ValueObject>();

                        Random::Type* type;

                        switch ((unsigned char)popStackAs<Value>()->value)
                        {
                            case 0x00:
                                type = new Random::Type(Random::TypeEnum::Step);

                                break;

                            case 0x01:
                                type = new Random::Type(Random::TypeEnum::Linear);

                                break;

                            default:
                                return Utils::machineError("Unrecognized constant code.", path);
                        }

                        stack.push(new Random(from, to, length, type));

                        address += 2;

                        break;
                    }

                    case 0x10:
                        stack.push(new Limit(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x11:
                        stack.push(new Delay(popStackAs<ValueObject>(), popStackAs<ValueObject>(), popStackAs<ValueObject>()));

                        address += 2;

                        break;

                    case 0x12:
                        // perform (get address, then call execute again at some point)

                        address += 2;

                        break;

                    default:
                        std::ostringstream code;

                        code << std::hex << program[address + 1];

                        return Utils::machineError("Unrecognized function code \"" + code.str() + "\"", path);
                }

                break;

            default:
                std::ostringstream code;

                code << std::hex << program[address];

                return Utils::machineError("Unrecognized instruction code \"" + code.str() + "\".", path);
        }
    }
}

void Machine::processAudioSources(double* buffer, const unsigned int bufferLength) const
{
    std::fill(buffer, buffer + bufferLength * utils->channels, 0);

    for (AudioSource* audioSource : audioSources)
    {
        audioSource->fillBuffer(buffer, bufferLength);
    }
}

void Machine::performEvents()
{
    for (int i = 0; i < events.size(); i++)
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

    bytes[0] = program[address];
    bytes[1] = program[address + 1];
    bytes[2] = program[address + 2];
    bytes[3] = program[address + 3];

    return *reinterpret_cast<unsigned int*>(bytes);
}

double Machine::readDouble(const unsigned int address) const
{
    unsigned char bytes[8];

    bytes[0] = program[address];
    bytes[1] = program[address + 1];
    bytes[2] = program[address + 2];
    bytes[3] = program[address + 3];
    bytes[4] = program[address + 4];
    bytes[5] = program[address + 5];
    bytes[6] = program[address + 6];
    bytes[7] = program[address + 7];

    return *reinterpret_cast<double*>(bytes);
}

Object* Machine::popStack()
{
    Object* value = stack.top();

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
    Object* object = popStack();

    std::vector<T*> objects;

    if (List<Object>* list = dynamic_cast<List<Object>*>(object))
    {
        for (Object* object : list->objects)
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
