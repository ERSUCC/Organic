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
    else if (function == "sample") id = BytecodeConstants::SAMPLE;

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

CallUser::CallUser(const InstructionBlock* function) :
    BytecodeInstruction(6), function(function) {}

void CallUser::output(std::ofstream& stream) const
{
    stream << BytecodeConstants::CALL_USER;

    writeUnsignedInt(stream, function->offset);

    stream << function->inputs;
}

ResourceBlock::ResourceBlock(const std::filesystem::path& path)
{
    SndfileHandle* file = new SndfileHandle(path.string());

    length = file->frames() * file->channels();
    sampleRate = file->samplerate();

    samples = (int*)malloc(sizeof(int) * length);

    if (file->read(samples, length) != length)
    {
        Utils::fileError("Could not read audio file \"" + path.string() + "\": " + std::string(file->strError()));
    }

    size = length * 4 + 4;
}

ResourceBlock::~ResourceBlock()
{
    free(samples);
}

void ResourceBlock::output(std::ofstream& stream) const
{
    writeUnsignedInt(stream, length);
    writeUnsignedInt(stream, sampleRate);

    for (unsigned int i = 0; i < length; i++)
    {
        writeInt(stream, samples[i]);
    }
}

InstructionBlock::InstructionBlock(const unsigned char inputs) :
    inputs(inputs)
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
