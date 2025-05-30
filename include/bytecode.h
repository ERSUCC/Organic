#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <sndfile.hh>

#include "constants.h"
#include "path.h"
#include "utils.h"

struct InstructionBlock;

void writeUnsignedInt(std::ofstream& stream, const unsigned int i);
void writeInt(std::ofstream& stream, const int i);
void writeDouble(std::ofstream& stream, const double d);

struct BytecodeInstruction
{
    BytecodeInstruction(const unsigned int size);

    virtual void output(std::ofstream& stream) const = 0;

    const unsigned int size;
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
    StackPushAddress(const InstructionBlock* block);

    void output(std::ofstream& stream) const override;

private:
    const InstructionBlock* block;

};

struct StackPushResource : public BytecodeInstruction
{
    StackPushResource(const unsigned char resource);

    void output(std::ofstream& stream) const override;

private:
    const unsigned char resource;

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
    CallNative(const unsigned char function, const unsigned char inputs);

    void output(std::ofstream& stream) const override;

private:
    const unsigned char function;
    const unsigned char inputs;

};

struct CallUser : public BytecodeInstruction
{
    CallUser(const InstructionBlock* function, const unsigned char inputs);

    void output(std::ofstream& stream) const override;

private:
    const InstructionBlock* function;

    const unsigned char inputs;

};

struct BytecodeBlock
{
    virtual void output(std::ofstream& stream) const = 0;

    unsigned int size;
};

struct ResourceBlock : BytecodeBlock
{
    ResourceBlock(const Path* path);
    ~ResourceBlock();

    void output(std::ofstream& stream) const override;

private:
    int* samples;

    unsigned int length;
    unsigned int sampleRate;
    unsigned int channels;

};

struct InstructionBlock : BytecodeBlock
{
    InstructionBlock();

    void output(std::ofstream& stream) const override;

    void addInstruction(const BytecodeInstruction* instruction);

    unsigned int offset;

private:
    std::vector<const BytecodeInstruction*> instructions;

};

struct BytecodeResolver
{
    void output(std::ofstream& stream, const unsigned char variables);

    void addResourceBlock(ResourceBlock* block);
    void addInstructionBlock(InstructionBlock* block);

private:
    std::vector<ResourceBlock*> resourceBlocks;
    std::vector<InstructionBlock*> instructionBlocks;

};
