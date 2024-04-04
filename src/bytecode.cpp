#include "../include/bytecode.h"

BytecodeInstruction::BytecodeInstruction(unsigned int size) :
    size(size) {}

std::vector<unsigned char> BytecodeInstruction::intToBytes(unsigned int i, unsigned int n) const
{
    std::vector<unsigned char> bytes;

    for (int j = 0; j < n; j++)
    {
        bytes.push_back((i >> (n * 8)) & 255);
    };

    return bytes;
}

std::vector<unsigned char> BytecodeInstruction::doubleToBytes(double d) const
{
    std::vector<unsigned char> bytes;

    // fill in bytes

    return bytes;
}

void EndBlock::output(std::ofstream& stream) const
{
    stream << 0x00;
}

void StackPush::output(std::ofstream& stream) const
{
    stream << 0x01;
}

void StackPop::output(std::ofstream& stream) const
{
    stream << 0x02;
}

SetVariable::SetVariable(std::string variable) :
    BytecodeInstruction(5), variable(variable) {}

void SetVariable::output(std::ofstream& stream) const
{
    stream << 0x03;

    unsigned int id = 0;

    // assign id

    for (unsigned char b : intToBytes(id, 1))
    {
        stream << b;
    }
}

GetVariable::GetVariable(std::string variable) :
    BytecodeInstruction(5), variable(variable) {}

void GetVariable::output(std::ofstream& stream) const
{
    stream << 0x04;

    unsigned int id = 0;

    // assign id

    for (unsigned char b : intToBytes(id, 1))
    {
        stream << b;
    }
}

GetVariableCopy::GetVariableCopy(std::string variable) :
    BytecodeInstruction(5), variable(variable) {}

void GetVariableCopy::output(std::ofstream& stream) const
{
    stream << 0x05;

    unsigned int id = 0;

    // assign id

    for (unsigned char b : intToBytes(id, 1))
    {
        stream << b;
    }
}

CallNative::CallNative(std::string function) :
    BytecodeInstruction(2), function(function) {}

void CallNative::output(std::ofstream& stream) const
{
    stream << 0x06;

    unsigned int id;

    if (function == "sine") id = 0;
    else if (function == "square") id = 1;
    else if (function == "triangle") id = 2;
    else if (function == "saw") id = 3;
    else if (function == "noise") id = 4;
    else if (function == "hold") id = 5;
    else if (function == "lfo") id = 6;
    else if (function == "sweep") id = 7;
    else if (function == "sequence") id = 8;
    else if (function == "repeat") id = 9;
    else if (function == "random") id = 10;
    else if (function == "delay") id = 11;
    else if (function == "perform") id = 12;

    for (unsigned char b : intToBytes(id, 1))
    {
        stream << b;
    }
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
    }
}
