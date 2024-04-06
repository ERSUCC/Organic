#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "utils.h"

struct BytecodeResolver;

struct BytecodeInstruction
{
    BytecodeInstruction(unsigned int size);

    virtual void output(std::ofstream& stream, BytecodeResolver* resolver) const = 0;

    unsigned int size;

protected:
    std::vector<unsigned char> intToBytes(unsigned int i) const;
    std::vector<unsigned char> doubleToBytes(double d) const;

};

struct StackPushByte : public BytecodeInstruction
{
    StackPushByte(const unsigned char value);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

    const unsigned char value;
};

struct StackPushInt : public BytecodeInstruction
{
    StackPushInt(const unsigned int value);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

    const unsigned int value;
};

struct StackPushDouble : public BytecodeInstruction
{
    StackPushDouble(const double value);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

    const double value;
};

struct SetVariable : public BytecodeInstruction
{
    SetVariable(const std::string variable);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

    const std::string variable;
};

struct GetVariable : public BytecodeInstruction
{
    GetVariable(std::string variable);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

    std::string variable;
};

struct GetVariableCopy : public BytecodeInstruction
{
    GetVariableCopy(std::string variable);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

    std::string variable;
};

struct CallNative : public BytecodeInstruction
{
    CallNative(std::string function);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

    std::string function;
};

struct BytecodeBlock
{
    std::vector<BytecodeInstruction*> instructions;

    unsigned int offset;
};

struct BytecodeResolver
{
    void output(const std::string path);

    std::vector<BytecodeBlock*> blocks;

    std::unordered_map<std::string, unsigned char> variables;
};
