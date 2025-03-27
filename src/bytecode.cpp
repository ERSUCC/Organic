#include "../include/bytecode.h"

void writeUnsignedInt(std::ofstream& stream, const unsigned int i)
{
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&i);

    if (Utils::get()->littleEndian)
    {
        stream << bytes[0] << bytes[1] << bytes[2] << bytes[3];
    }

    else
    {
        stream << bytes[3] << bytes[2] << bytes[1] << bytes[0];
    }
}

void writeInt(std::ofstream& stream, const int i)
{
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&i);

    if (Utils::get()->littleEndian)
    {
        stream << bytes[0] << bytes[1] << bytes[2] << bytes[3];
    }

    else
    {
        stream << bytes[3] << bytes[2] << bytes[1] << bytes[0];
    }
}

void writeDouble(std::ofstream& stream, const double d)
{
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(&d);

    if (Utils::get()->littleEndian)
    {
        stream << bytes[0] << bytes[1] << bytes[2] << bytes[3] << bytes[4] << bytes[5] << bytes[6] << bytes[7];
    }

    else
    {
        stream << bytes[7] << bytes[6] << bytes[5] << bytes[4] << bytes[3] << bytes[2] << bytes[1] << bytes[0];
    }
}

BytecodeInstruction::BytecodeInstruction(const unsigned int size) :
    size(size) {}

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

    writeUnsignedInt(stream, value);
}

StackPushDouble::StackPushDouble(const double value) :
    BytecodeInstruction(9), value(value) {}

void StackPushDouble::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::STACK_PUSH_DOUBLE;

    writeDouble(stream, value);
}

StackPushAddress::StackPushAddress(const InstructionBlock* block) :
    BytecodeInstruction(5), block(block) {}

void StackPushAddress::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::STACK_PUSH_ADDRESS;

    writeUnsignedInt(stream, block->offset);
}

StackPushResource::StackPushResource(unsigned char resource) :
    BytecodeInstruction(2), resource(resource) {}

void StackPushResource::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::STACK_PUSH_RESOURCE << resource;
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

CallNative::CallNative(const unsigned char function, const unsigned char inputs) :
    BytecodeInstruction(3), function(function), inputs(inputs) {}

void CallNative::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::CALL_NATIVE << function << inputs;
}

CallUser::CallUser(const InstructionBlock* function, const unsigned char inputs) :
    BytecodeInstruction(6), function(function), inputs(inputs) {}

void CallUser::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::CALL_USER;

    writeUnsignedInt(stream, function->offset);

    stream << inputs;
}

ClearStack::ClearStack() :
    BytecodeInstruction(1) {}

void ClearStack::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::CLEAR_STACK;
}

ResourceBlock::ResourceBlock(const Path* path)
{
    SndfileHandle* file = new SndfileHandle(path->string());

    length = file->frames() * file->channels();
    sampleRate = file->samplerate();
    channels = file->channels();

    samples = (int*)malloc(sizeof(int) * length);

    if (file->read(samples, length) != length)
    {
        throw OrganicFileException("Could not read audio file \"" + path->string() + "\": " + std::string(file->strError()));
    }

    size = 12 + length * 4;
}

ResourceBlock::~ResourceBlock()
{
    free(samples);
}

void ResourceBlock::output(std::ofstream& stream) const
{
    writeUnsignedInt(stream, length);
    writeUnsignedInt(stream, sampleRate);
    writeUnsignedInt(stream, channels);

    for (unsigned int i = 0; i < length; i++)
    {
        writeInt(stream, samples[i]);
    }
}

InstructionBlock::InstructionBlock()
{
    size = 1;
}

void InstructionBlock::output(std::ofstream& stream) const
{
    for (const BytecodeInstruction* instruction : instructions)
    {
        instruction->output(stream);
    }

    stream << BytecodeConstants::RETURN;
}

void InstructionBlock::addInstruction(const BytecodeInstruction* instruction)
{
    instructions.push_back(instruction);

    size += instruction->size;
}

void BytecodeResolver::output(std::ofstream& stream, const unsigned char variables)
{
    stream << BytecodeConstants::OBC_ID << variables << (unsigned char)resourceBlocks.size();

    unsigned int offset = BytecodeConstants::HEADER_LENGTH;

    for (ResourceBlock* block : resourceBlocks)
    {
        offset += block->size;
    }

    writeUnsignedInt(stream, offset);

    offset = 0;

    for (InstructionBlock* block : instructionBlocks)
    {
        block->offset = offset;

        offset += block->size;
    }

    for (ResourceBlock* block : resourceBlocks)
    {
        block->output(stream);
    }

    for (InstructionBlock* block : instructionBlocks)
    {
        block->output(stream);
    }
}

void BytecodeResolver::addResourceBlock(ResourceBlock* block)
{
    resourceBlocks.push_back(block);
}

void BytecodeResolver::addInstructionBlock(InstructionBlock* block)
{
    instructionBlocks.push_back(block);
}
