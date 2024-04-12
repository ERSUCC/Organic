#include "../include/bytecode.h"

BytecodeInstruction::BytecodeInstruction(const unsigned int size) :
    size(size) {}

std::vector<unsigned char> BytecodeInstruction::intToBytes(const unsigned int i) const
{
    const unsigned char* bytes_array = reinterpret_cast<const unsigned char*>(&i);

    std::vector<unsigned char> bytes;

    for (unsigned int j = 0; j < 4; j++)
    {
        bytes.push_back(bytes_array[j]);
    }

    return bytes;
}

std::vector<unsigned char> BytecodeInstruction::doubleToBytes(const double d) const
{
    const unsigned char* bytes_array = reinterpret_cast<const unsigned char*>(&d);

    std::vector<unsigned char> bytes;

    for (unsigned int i = 0; i < 8; i++)
    {
        bytes.push_back(bytes_array[i]);
    }

    return bytes;
}

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

    stream << (unsigned char)0x04 << id;
}

GetVariable::GetVariable(const std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void GetVariable::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x05 << resolver->variables[variable];
}

GetVariableCopy::GetVariableCopy(const std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void GetVariableCopy::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x06 << resolver->variables[variable];
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

    stream << (unsigned char)0x07 << id;
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
