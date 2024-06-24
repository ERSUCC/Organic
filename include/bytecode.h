#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils.h"

struct BytecodeResolver;
struct BytecodeBlock;

std::vector<unsigned char> intToBytes(const unsigned int i);
std::vector<unsigned char> doubleToBytes(const double d);

struct BytecodeInstruction
{
    BytecodeInstruction(const unsigned int size);

    virtual void output(std::ofstream& stream, BytecodeResolver* resolver) const = 0;

    const unsigned int size;
};

struct StackPushByte : public BytecodeInstruction
{
    StackPushByte(const unsigned char value);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const unsigned char value;

};

struct StackPushInt : public BytecodeInstruction
{
    StackPushInt(const unsigned int value);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const unsigned int value;

};

struct StackPushDouble : public BytecodeInstruction
{
    StackPushDouble(const double value);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const double value;

};

struct StackPushAddress : public BytecodeInstruction
{
    StackPushAddress(const BytecodeBlock* block);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const BytecodeBlock* block;

};

struct SetVariable : public BytecodeInstruction
{
    SetVariable(const unsigned char variable);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const unsigned char variable;

};

struct GetVariable : public BytecodeInstruction
{
    GetVariable(const unsigned char variable);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const unsigned char variable;

};

struct CallNative : public BytecodeInstruction
{
    CallNative(const std::string function, const unsigned char inputs);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const std::string function;

    const unsigned char inputs;

};

struct CallUser : public BytecodeInstruction
{
    CallUser(const BytecodeBlock* function, const unsigned char inputs);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const BytecodeBlock* function;

    const unsigned char inputs;

};

struct SetInput : public BytecodeInstruction
{
    SetInput(const unsigned char input);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const unsigned char input;

};

struct GetInput : public BytecodeInstruction
{
    GetInput(const unsigned char input);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const override;

private:
    const unsigned char input;

};

struct BytecodeBlock
{
    BytecodeBlock(const std::vector<std::string> inputs);

    void addInstruction(const BytecodeInstruction* instruction);

    void output(std::ofstream& stream, BytecodeResolver* resolver) const;

    const std::vector<std::string> inputs;

    unsigned int offset;
    unsigned int size = 1;

private:
    std::vector<const BytecodeInstruction*> instructions;

};

struct BytecodeResolver
{
    void output(const std::string path);

    void addBlock(BytecodeBlock* block);

    std::unordered_map<std::string, unsigned char> variables;

private:
    std::vector<BytecodeBlock*> blocks;

};
