#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "constants.h"
#include "utils.h"

struct BytecodeBlock;

std::vector<unsigned char> intToBytes(const unsigned int i);
std::vector<unsigned char> doubleToBytes(const double d);

struct BytecodeInstruction
{
    BytecodeInstruction(const unsigned int size);

    virtual void output(std::ofstream& stream) const = 0;

    const unsigned int size;
};

struct StackPushDefault : public BytecodeInstruction
{
    StackPushDefault();

    void output(std::ofstream& stream) const override;
};

struct StackPushByte : public BytecodeInstruction
{
    StackPushByte(const unsigned char value);

    void output(std::ofstream& stream) const override;

private:
    const unsigned char value;

};

struct StackPushInt : public BytecodeInstruction
{
    StackPushInt(const unsigned int value);

    void output(std::ofstream& stream) const override;

private:
    const unsigned int value;

};

struct StackPushDouble : public BytecodeInstruction
{
    StackPushDouble(const double value);

    void output(std::ofstream& stream) const override;

private:
    const double value;

};

struct StackPushAddress : public BytecodeInstruction
{
    StackPushAddress(const BytecodeBlock* block);

    void output(std::ofstream& stream) const override;

private:
    const BytecodeBlock* block;

};

struct SetVariable : public BytecodeInstruction
{
    SetVariable(const unsigned char variable);

    void output(std::ofstream& stream) const override;

private:
    const unsigned char variable;

};

struct GetVariable : public BytecodeInstruction
{
    GetVariable(const unsigned char variable);

    void output(std::ofstream& stream) const override;

private:
    const unsigned char variable;

};

struct CallNative : public BytecodeInstruction
{
    CallNative(const std::string function, const unsigned char inputs);

    void output(std::ofstream& stream) const override;

private:
    const std::string function;

    const unsigned char inputs;

};

struct CallUser : public BytecodeInstruction
{
    CallUser(const BytecodeBlock* function);

    void output(std::ofstream& stream) const override;

private:
    const BytecodeBlock* function;

};

struct BytecodeBlock
{
    BytecodeBlock(const unsigned char inputs);

    void addInstruction(const BytecodeInstruction* instruction);

    void output(std::ofstream& stream) const;

    const unsigned char inputs;

    unsigned int offset;
    unsigned int size = 1;

private:
    std::vector<const BytecodeInstruction*> instructions;

};

struct BytecodeResolver
{
    void output(const std::string path, const unsigned char variables);

    void addBlock(BytecodeBlock* block);

private:
    std::vector<BytecodeBlock*> blocks;

};
