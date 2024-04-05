#include "../include/machine.h"

unsigned int ObjectPool::allocate(Object* object)
{
    unsigned int address;

    if (free.empty())
    {
        address = end++;
    }

    else
    {
        address = *free.begin();

        free.erase(address);
    }

    heap[address] = object;

    return address;
}

void ObjectPool::deallocate(unsigned int address)
{
    free.insert(address);

    int last = *free.rbegin();

    if (last < end)
    {
        end = *free.rbegin();
    }
}

Machine::Machine(const std::string path) : path(path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        Utils::error("Could not open \"" + path + "\".");
    }

    std::getline(file, program, std::string::traits_type::to_char_type(std::string::traits_type::eof()));

    if (program.size() <= 4 || program.substr(0, 4) != "BACH")
    {
        Utils::machineError("Invalid bytecode.", path);
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
                // stack push byte

                address += 2;

                break;

            case 0x02:
                // stack push int

                address += 5;

                break;

            case 0x03:
                // stack push double

                address += 9;

                break;

            case 0x04:
                // set variable

                address += 2;

                break;

            case 0x05:
                // get variable

                address += 2;

                break;

            case 0x06:
                // get variable copy

                address += 6;

            case 0x07:
                // call native

                address += 2;

                break;

            default:
                std::ostringstream code;

                code << std::hex << program[address];

                return Utils::machineError("Unrecognized instruction code \"" + code.str() + "\".", path);
        }
    }
}
