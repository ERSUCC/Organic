#pragma once

#include <string>
#include <vector>
#include <fstream>

struct BytecodeInstruction
{
    BytecodeInstruction(unsigned int size);

    virtual void output(std::ofstream& stream) const = 0;

    unsigned int size;

protected:
    std::vector<unsigned char> intToBytes(unsigned int i, unsigned int n) const;
    std::vector<unsigned char> doubleToBytes(double d) const;

};

struct EndBlock : public BytecodeInstruction
{
    void output(std::ofstream& stream) const override;
};

struct StackPush : public BytecodeInstruction
{
    void output(std::ofstream& stream) const override;
};

struct StackPop : public BytecodeInstruction
{
    void output(std::ofstream& stream) const override;
};

struct SetVariable : public BytecodeInstruction
{
    SetVariable(std::string variable);

    void output(std::ofstream& stream) const override;

    std::string variable;
};

struct GetVariable : public BytecodeInstruction
{
    GetVariable(std::string variable);

    void output(std::ofstream& stream) const override;

    std::string variable;
};

struct GetVariableCopy : public BytecodeInstruction
{
    GetVariableCopy(std::string variable);

    void output(std::ofstream& stream) const override;

    std::string variable;
};

struct CallNative : public BytecodeInstruction
{
    CallNative(std::string function);

    void output(std::ofstream& stream) const override;

    std::string function;
};

struct BytecodeBlock
{
    std::vector<BytecodeInstruction*> instructions;

    unsigned int offset;
};

struct BytecodeResolver
{
    void output(std::ofstream& stream);

    std::vector<BytecodeBlock*> blocks;
};
