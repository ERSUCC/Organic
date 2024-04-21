#include "../include/bytecode.h"

std::vector<unsigned char> intToBytes(const unsigned int i)
{
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&i);

    if (Utils::get()->littleEndian)
    {
        return std::vector<unsigned char>
        {
            bytes[0],
            bytes[1],
            bytes[2],
            bytes[3]
        };
    }
    
    return std::vector<unsigned char>
    {
        bytes[3],
        bytes[2],
        bytes[1],
        bytes[0]
    };
}

std::vector<unsigned char> doubleToBytes(const double d)
{
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&d);

    if (Utils::get()->littleEndian)
    {
        return std::vector<unsigned char>
        {
            bytes[0],
            bytes[1],
            bytes[2],
            bytes[3],
            bytes[4],
            bytes[5],
            bytes[6],
            bytes[7]
        };
    }

    return std::vector<unsigned char>
    {
        bytes[7],
        bytes[6],
        bytes[5],
        bytes[4],
        bytes[3],
        bytes[2],
        bytes[1],
        bytes[0]
    };
}

BytecodeInstruction::BytecodeInstruction(const unsigned int size) :
    size(size) {}

StackPushByte::StackPushByte(const unsigned char value) :
    BytecodeInstruction(2), value(value) {}

void StackPushByte::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x01 << value;
}

StackPushInt::StackPushInt(const unsigned int value) :
    BytecodeInstruction(5), value(value) {}

void StackPushInt::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x02;

    for (const unsigned char b : intToBytes(value))
    {
        stream << b;
    }
}

StackPushDouble::StackPushDouble(const double value) :
    BytecodeInstruction(9), value(value) {}

void StackPushDouble::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x03;

    for (const unsigned char b : doubleToBytes(value))
    {
        stream << b;
    }
}

StackPushAddress::StackPushAddress(const BytecodeBlock* block) :
    BytecodeInstruction(5), block(block) {}

void StackPushAddress::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x04;

    for (const unsigned char b : intToBytes(block->offset))
    {
        stream << b;
    }
}

SetVariable::SetVariable(const std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void SetVariable::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    unsigned char id = 0;

    if (resolver->variables.count(variable))
    {
        id = resolver->variables[variable];
    }

    else
    {
        id = resolver->variables.size();

        resolver->variables[variable] = id;
    }

    stream << (unsigned char)0x05 << id;
}

GetVariable::GetVariable(const std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void GetVariable::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x06 << resolver->variables[variable];
}

GetVariableCopy::GetVariableCopy(const std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void GetVariableCopy::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x07 << resolver->variables[variable];
}

CallNative::CallNative(std::string function) :
    BytecodeInstruction(2), function(function) {}

void CallNative::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    unsigned char id;

    if (function == "list") id = 0x00;

    else if (function == "add") id = 0x10;
    else if (function == "subtract") id = 0x11;
    else if (function == "multiply") id = 0x12;
    else if (function == "divide") id = 0x13;
    else if (function == "equal") id = 0x14;
    else if (function == "less") id = 0x15;
    else if (function == "greater") id = 0x16;
    else if (function == "lessequal") id = 0x17;
    else if (function == "greaterequal") id = 0x18;

    else if (function == "sine") id = 0x30;
    else if (function == "square") id = 0x31;
    else if (function == "triangle") id = 0x32;
    else if (function == "saw") id = 0x33;
    else if (function == "noise") id = 0x34;

    else if (function == "hold") id = 0x50;
    else if (function == "lfo") id = 0x51;
    else if (function == "sweep") id = 0x52;
    else if (function == "sequence") id = 0x53;
    else if (function == "repeat") id = 0x54;
    else if (function == "random") id = 0x55;
    else if (function == "limit") id = 0x56;
    else if (function == "trigger") id = 0x57;
    else if (function == "delay") id = 0x58;

    else if (function == "perform") id = 0x70;

    stream << (unsigned char)0x08 << id;
}

void BytecodeResolver::output(const std::string path)
{
    std::ofstream stream(path, std::ios::binary);

    if (!stream.is_open())
    {
        return Utils::error("Could not create intermediate file \"" + path + "\".");
    }

    stream << "BACH";

    unsigned int offset = 4;

    for (BytecodeBlock* block : blocks)
    {
        block->offset = offset;

        for (BytecodeInstruction* instruction : block->instructions)
        {
            offset += instruction->size;
        }

        offset++;
    }

    for (BytecodeBlock* block : blocks)
    {
        for (BytecodeInstruction* instruction : block->instructions)
        {
            instruction->output(stream, this);
        }

        stream << (unsigned char)0x00;
    }

    stream.close();
}
