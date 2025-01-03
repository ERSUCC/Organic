#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <sndfile.hh>

#include "constants.h"
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
    CallNative(const std::string function, const unsigned char inputs);

    void output(std::ofstream& stream) const override;

private:
    const std::string function;

    const unsigned char inputs;

};

struct CallUser : public BytecodeInstruction
{
    CallUser(const InstructionBlock* function);

    void output(std::ofstream& stream) const override;

private:
    const InstructionBlock* function;

};

struct BytecodeBlock
{
    virtual void output(std::ofstream& stream) const = 0;

    unsigned int size;
};

struct ResourceBlock : BytecodeBlock
{
    ResourceBlock(const std::filesystem::path& path);
    ~ResourceBlock();

    void output(std::ofstream& stream) const override;

private:
    int* samples;

    unsigned int length;
    unsigned int sampleRate;

};

struct InstructionBlock : BytecodeBlock
{
    InstructionBlock(const unsigned char inputs);

    void output(std::ofstream& stream) const override;

    void addInstruction(const BytecodeInstruction* instruction);

    const unsigned char inputs;

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
