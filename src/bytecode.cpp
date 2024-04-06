#include "../include/bytecode.h"

BytecodeInstruction::BytecodeInstruction(unsigned int size) :
    size(size) {}

std::vector<unsigned char> BytecodeInstruction::intToBytes(unsigned int i) const
{
    unsigned char* bytes_array = reinterpret_cast<unsigned char*>(&i);

    std::vector<unsigned char> bytes;

    for (unsigned int j = 0; j < 4; j++)
    {
        bytes.push_back(bytes_array[j]);
    }

    return bytes;
}

std::vector<unsigned char> BytecodeInstruction::doubleToBytes(double d) const
{
    unsigned char* bytes_array = reinterpret_cast<unsigned char*>(&d);

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

    for (unsigned char b : intToBytes(value))
    {
        stream << b;
    }
}

StackPushDouble::StackPushDouble(const double value) :
    BytecodeInstruction(9), value(value) {}

void StackPushDouble::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x03;

    for (unsigned char b : doubleToBytes(value))
    {
        stream << b;
    }
}

SetVariable::SetVariable(std::string variable) :
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

GetVariable::GetVariable(std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void GetVariable::output(std::ofstream& stream, BytecodeResolver* resolver) const
{
    stream << (unsigned char)0x05 << resolver->variables[variable];
}

GetVariableCopy::GetVariableCopy(std::string variable) :
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
    else if (function == "add") id = 0x01;
    else if (function == "subtract") id = 0x02;
    else if (function == "multiply") id = 0x03;
    else if (function == "divide") id = 0x04;
    else if (function == "sine") id = 0x05;
    else if (function == "square") id = 0x06;
    else if (function == "triangle") id = 0x07;
    else if (function == "saw") id = 0x08;
    else if (function == "noise") id = 0x09;
    else if (function == "hold") id = 0x0a;
    else if (function == "lfo") id = 0x0b;
    else if (function == "sweep") id = 0x0c;
    else if (function == "sequence") id = 0x0d;
    else if (function == "repeat") id = 0x0e;
    else if (function == "random") id = 0x0f;
    else if (function == "limit") id = 0x10;
    else if (function == "delay") id = 0x11;
    else if (function == "perform") id = 0x12;

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
