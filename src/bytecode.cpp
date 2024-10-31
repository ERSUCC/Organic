#include "../include/bytecode.h"

std::vector<unsigned char> intToBytes(const unsigned int i)
{
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&i);

    if (Utils::get()->littleEndian)
    {
        return std::vector<unsigned char>
        {
            bytes[0],
            bytes[1],
            bytes[2],
            bytes[3]
        };
    }
    
    return std::vector<unsigned char>
    {
        bytes[3],
        bytes[2],
        bytes[1],
        bytes[0]
    };
}

std::vector<unsigned char> doubleToBytes(const double d)
{
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&d);

    if (Utils::get()->littleEndian)
    {
        return std::vector<unsigned char>
        {
            bytes[0],
            bytes[1],
            bytes[2],
            bytes[3],
            bytes[4],
            bytes[5],
            bytes[6],
            bytes[7]
        };
    }

    return std::vector<unsigned char>
    {
        bytes[7],
        bytes[6],
        bytes[5],
        bytes[4],
        bytes[3],
        bytes[2],
        bytes[1],
        bytes[0]
    };
}

BytecodeInstruction::BytecodeInstruction(const unsigned int size) :
    size(size) {}

StackPushDefault::StackPushDefault() :
    BytecodeInstruction(1) {}

void StackPushDefault::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::STACK_PUSH_DEFAULT;
}

StackPushByte::StackPushByte(const unsigned char value) :
    BytecodeInstruction(2), value(value) {}

void StackPushByte::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::STACK_PUSH_BYTE << value;
}

StackPushInt::StackPushInt(const unsigned int value) :
    BytecodeInstruction(5), value(value) {}

void StackPushInt::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::STACK_PUSH_INT;

    for (const unsigned char b : intToBytes(value))
    {
        stream << b;
    }
}

StackPushDouble::StackPushDouble(const double value) :
    BytecodeInstruction(9), value(value) {}

void StackPushDouble::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::STACK_PUSH_DOUBLE;

    for (const unsigned char b : doubleToBytes(value))
    {
        stream << b;
    }
}

StackPushAddress::StackPushAddress(const BytecodeBlock* block) :
    BytecodeInstruction(5), block(block) {}

void StackPushAddress::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::STACK_PUSH_ADDRESS;

    for (const unsigned char b : intToBytes(block->offset))
    {
        stream << b;
    }
}

SetVariable::SetVariable(const unsigned char variable) :
    BytecodeInstruction(2), variable(variable) {}

void SetVariable::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::SET_VARIABLE << variable;
}

GetVariable::GetVariable(const unsigned char variable) :
    BytecodeInstruction(2), variable(variable) {}

void GetVariable::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::GET_VARIABLE << variable;
}

CallNative::CallNative(const std::string function, const unsigned char inputs) :
    BytecodeInstruction(3), function(function), inputs(inputs) {}

void CallNative::output(std::ofstream& stream) const
{
    unsigned char id;

    if (function == "list") id = BytecodeConstants::LIST;
    else if (function == "time") id = BytecodeConstants::TIME;

    else if (function == "add") id = BytecodeConstants::ADD;
    else if (function == "subtract") id = BytecodeConstants::SUBTRACT;
    else if (function == "multiply") id = BytecodeConstants::MULTIPLY;
    else if (function == "divide") id = BytecodeConstants::DIVIDE;
    else if (function == "power") id = BytecodeConstants::POWER;
    else if (function == "equal") id = BytecodeConstants::EQUAL;
    else if (function == "less") id = BytecodeConstants::LESS;
    else if (function == "greater") id = BytecodeConstants::GREATER;
    else if (function == "lessequal") id = BytecodeConstants::LESSEQUAL;
    else if (function == "greaterequal") id = BytecodeConstants::GREATEREQUAL;

    else if (function == "sine") id = BytecodeConstants::SINE;
    else if (function == "square") id = BytecodeConstants::SQUARE;
    else if (function == "triangle") id = BytecodeConstants::TRIANGLE;
    else if (function == "saw") id = BytecodeConstants::SAW;
    else if (function == "noise") id = BytecodeConstants::NOISE;
    else if (function == "blend") id = BytecodeConstants::BLEND;

    else if (function == "hold") id = BytecodeConstants::HOLD;
    else if (function == "lfo") id = BytecodeConstants::LFO;
    else if (function == "sweep") id = BytecodeConstants::SWEEP;
    else if (function == "sequence") id = BytecodeConstants::SEQUENCE;
    else if (function == "repeat") id = BytecodeConstants::REPEAT;
    else if (function == "random") id = BytecodeConstants::RANDOM;
    else if (function == "limit") id = BytecodeConstants::LIMIT;
    else if (function == "trigger") id = BytecodeConstants::TRIGGER;
    else if (function == "if") id = BytecodeConstants::IF;

    else if (function == "delay") id = BytecodeConstants::DELAY;

    else if (function == "play") id = BytecodeConstants::PLAY;
    else if (function == "perform") id = BytecodeConstants::PERFORM;

    stream << BytecodeConstants::CALL_NATIVE << id << inputs;
}

CallUser::CallUser(const BytecodeBlock* function) :
    BytecodeInstruction(6), function(function) {}

void CallUser::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::CALL_USER;

    for (const unsigned char b : intToBytes(function->offset))
    {
        stream << b;
    }

    stream << function->inputs;
}

BytecodeBlock::BytecodeBlock(const unsigned char inputs) :
    inputs(inputs) {}

void BytecodeBlock::addInstruction(const BytecodeInstruction* instruction)
{
    instructions.push_back(instruction);

    size += instruction->size;
}

void BytecodeBlock::output(std::ofstream& stream) const
{
    for (const BytecodeInstruction* instruction : instructions)
    {
        instruction->output(stream);
    }

    stream << BytecodeConstants::RETURN;
}

void BytecodeResolver::output(const std::string path, const unsigned char variables)
{
    std::ofstream stream(path, std::ios::binary);

    if (!stream.is_open())
    {
        return Utils::error("Error creating intermediate file \"" + path + "\".");
    }

    stream << BytecodeConstants::OBC_ID << variables;

    unsigned int offset = BytecodeConstants::HEADER_LENGTH;

    for (BytecodeBlock* block : blocks)
    {
        block->offset = offset;

        offset += block->size;
    }

    for (BytecodeBlock* block : blocks)
    {
        block->output(stream);
    }

    stream.close();
}

void BytecodeResolver::addBlock(BytecodeBlock* block)
{
    blocks.push_back(block);
}
