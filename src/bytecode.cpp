#include "../include/bytecode.h"

BytecodeInstruction::BytecodeInstruction(unsigned int size) :
    size(size) {}

std::vector<unsigned char> BytecodeInstruction::intToBytes(unsigned int i, unsigned int n) const
{
    std::vector<unsigned char> bytes;

    for (int j = 0; j < n; j++)
    {
        // this probably gets funky with different endianness and bit width, check on it later

        bytes.push_back((i >> (n * 8)) & 255);
    };

    return bytes;
}

std::vector<unsigned char> BytecodeInstruction::doubleToBytes(double d) const
{
    std::vector<unsigned char> bytes;

    // fill in bytes, this is dummy for testing

    for (int i = 0; i < 8; i++)
    {
        bytes.push_back(0);
    }

    return bytes;
}

StackPushByte::StackPushByte(const unsigned char value) :
    BytecodeInstruction(2), value(value) {}

void StackPushByte::output(std::ofstream& stream) const
{
    stream << (unsigned char)0x01 << value;
}

StackPushInt::StackPushInt(const unsigned int value) :
    BytecodeInstruction(5), value(value) {}

void StackPushInt::output(std::ofstream& stream) const
{
    stream << (unsigned char)0x02;

    for (unsigned char b : intToBytes(value, 4))
    {
        stream << b;
    }
}

StackPushDouble::StackPushDouble(const double value) :
    BytecodeInstruction(9), value(value) {}

void StackPushDouble::output(std::ofstream& stream) const
{
    stream << (unsigned char)0x03;

    for (unsigned char b : doubleToBytes(value))
    {
        stream << b;
    }
}

SetVariable::SetVariable(std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void SetVariable::output(std::ofstream& stream) const
{
    unsigned int id = 0;

    // get id

    stream << (unsigned char)0x04 << intToBytes(id, 1)[0];
}

GetVariable::GetVariable(std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void GetVariable::output(std::ofstream& stream) const
{
    unsigned int id = 0;

    // get id

    stream << (unsigned char)0x05 << intToBytes(id, 1)[0];
}

GetVariableCopy::GetVariableCopy(std::string variable) :
    BytecodeInstruction(2), variable(variable) {}

void GetVariableCopy::output(std::ofstream& stream) const
{
    unsigned int id = 0;

    // get id

    stream << (unsigned char)0x06 << intToBytes(id, 1)[0];
}

CallNative::CallNative(std::string function) :
    BytecodeInstruction(2), function(function) {}

void CallNative::output(std::ofstream& stream) const
{
    unsigned int id;

    if (function == "value") id = 0;
    else if (function == "list") id = 1;
    else if (function == "add") id = 2;
    else if (function == "subtract") id = 3;
    else if (function == "multiply") id = 4;
    else if (function == "divide") id = 5;
    else if (function == "sine") id = 6;
    else if (function == "square") id = 7;
    else if (function == "triangle") id = 8;
    else if (function == "saw") id = 9;
    else if (function == "noise") id = 10;
    else if (function == "hold") id = 11;
    else if (function == "lfo") id = 12;
    else if (function == "sweep") id = 13;
    else if (function == "sequence") id = 14;
    else if (function == "repeat") id = 15;
    else if (function == "random") id = 16;
    else if (function == "delay") id = 17;
    else if (function == "perform") id = 18;

    stream << (unsigned char)0x07 << intToBytes(id, 1)[0];
}

void BytecodeResolver::output(std::ofstream& stream)
{
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
            instruction->output(stream);
        }

        stream << (unsigned char)0x00;
    }
}
