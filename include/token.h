#pragma once

#include <fstream>
#include <stack>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "bytecode.h"
#include "constants.h"
#include "controller.h"
#include "location.h"
#include "path.h"
#include "utils.h"

namespace Parser
{
    struct BytecodeTransformer;

    enum class BasicType
    {
        None,
        Any,
        SequenceOrder,
        RandomType,
        Number,
        Boolean,
        String,
        List,
        AudioSource,
        Effect
    };

    struct Type
    {
        Type(const BasicType primaryType, Type* subType = nullptr);

        std::string name() const;

        bool checkType(const Type* expected) const;

        Type* subType;

    private:
        const BasicType primaryType;

    };

    struct Token
    {
        Token(const SourceLocation location);

        virtual void resolveTypes(BytecodeTransformer* visitor);
        virtual void transform(BytecodeTransformer* visitor) const;

        const SourceLocation location;

        Type* type = nullptr;
    };

    struct BasicToken : public Token
    {
        BasicToken(const SourceLocation location, const std::string str);

        const std::string str;
    };

    struct OpenParenthesis : public BasicToken
    {
        OpenParenthesis(const SourceLocation location);
    };

    struct CloseParenthesis : public BasicToken
    {
        CloseParenthesis(const SourceLocation location);
    };

    struct OpenSquareBracket : public BasicToken
    {
        OpenSquareBracket(const SourceLocation location);
    };

    struct CloseSquareBracket : public BasicToken
    {
        CloseSquareBracket(const SourceLocation location);
    };

    struct OpenCurlyBracket : public BasicToken
    {
        OpenCurlyBracket(const SourceLocation location);
    };

    struct CloseCurlyBracket : public BasicToken
    {
        CloseCurlyBracket(const SourceLocation location);
    };

    struct Colon : public BasicToken
    {
        Colon(const SourceLocation location);
    };

    struct Comma : public BasicToken
    {
        Comma(const SourceLocation location);
    };

    struct Equals : public BasicToken
    {
        Equals(const SourceLocation location);
    };

    struct Operator : public BasicToken
    {
        Operator(const SourceLocation location, const std::string str);
    };

    struct Add : public Operator
    {
        Add(const SourceLocation location);
    };

    struct Subtract : public Operator
    {
        Subtract(const SourceLocation location);
    };

    struct Multiply : public Operator
    {
        Multiply(const SourceLocation location);
    };

    struct Divide : public Operator
    {
        Divide(const SourceLocation location);
    };

    struct Power : public Operator
    {
        Power(const SourceLocation location);
    };

    struct DoubleEquals : public Operator
    {
        DoubleEquals(const SourceLocation location);
    };

    struct Less : public Operator
    {
        Less(const SourceLocation location);
    };

    struct Greater : public Operator
    {
        Greater(const SourceLocation location);
    };

    struct LessEqual : public Operator
    {
        LessEqual(const SourceLocation location);
    };

    struct GreaterEqual : public Operator
    {
        GreaterEqual(const SourceLocation location);
    };

    struct Identifier : public BasicToken
    {
        Identifier(const SourceLocation location, const std::string str);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;

        unsigned char id;
    };

    struct Value : public BasicToken
    {
        Value(const SourceLocation location, const std::string str, const double value);

        void transform(BytecodeTransformer* visitor) const override;

        const double value;
    };

    struct SequenceForwards : public BasicToken
    {
        SequenceForwards(const SourceLocation location);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct SequenceBackwards : public BasicToken
    {
        SequenceBackwards(const SourceLocation location);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct SequencePingPong : public BasicToken
    {
        SequencePingPong(const SourceLocation location);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct SequenceRandom : public BasicToken
    {
        SequenceRandom(const SourceLocation location);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct RandomStep : public BasicToken
    {
        RandomStep(const SourceLocation location);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct RandomLinear : public BasicToken
    {
        RandomLinear(const SourceLocation location);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Pi : public BasicToken
    {
        Pi(const SourceLocation location);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct E : public BasicToken
    {
        E(const SourceLocation location);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct String : public BasicToken
    {
        String(const SourceLocation location, const std::string str);
    };

    struct Ignore : public Token
    {
        Ignore(const SourceLocation);
    };

    struct Argument : public Token
    {
        Argument(const SourceLocation location, const std::string name, Token* value);

        const std::string name;

        Token* value;

        bool used = false;
    };

    struct ArgumentList
    {
        ArgumentList(const std::vector<Argument*>& arguments, const std::string name);

        const std::vector<Argument*> arguments;

        const std::string name;
    };

    struct List : public Token
    {
        List(const SourceLocation location, const std::vector<Token*> values);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;

        const std::vector<Token*> values;
    };

    struct ParenthesizedExpression : public Token
    {
        ParenthesizedExpression(const SourceLocation, Token* value);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;

        Token* value;
    };

    struct Assign : public Token
    {
        Assign(const SourceLocation location, Identifier* variable, Token* value);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;

        Identifier* variable;

        Token* value;
    };

    struct Call : public Token
    {
        Call(const SourceLocation location, const ArgumentList* arguments, const bool topLevel);

        const ArgumentList* arguments;

        const bool topLevel;
    };

    struct Time : public Call
    {
        Time(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Hold : public Call
    {
        Hold(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct LFO : public Call
    {
        LFO(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Sweep : public Call
    {
        Sweep(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Sequence : public Call
    {
        Sequence(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Repeat : public Call
    {
        Repeat(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Random : public Call
    {
        Random(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Limit : public Call
    {
        Limit(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Trigger : public Call
    {
        Trigger(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct If : public Call
    {
        If(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Sine : public Call
    {
        Sine(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Square : public Call
    {
        Square(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Triangle : public Call
    {
        Triangle(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Saw : public Call
    {
        Saw(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Noise : public Call
    {
        Noise(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Sample : public Call
    {
        Sample(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Delay : public Call
    {
        Delay(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    // low pass filter goes here when it's ready

    struct Perform : public Call
    {
        Perform(const SourceLocation, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Define;

    struct CallUser : public Call
    {
        CallUser(const SourceLocation, const std::string name, const ArgumentList* arguments, const bool topLevel);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;

        const std::string name;

        Define* function;
    };

    struct CallAlias : public Token
    {
        CallAlias(const SourceLocation location, Token* a, Token* b);

        void resolveTypes(BytecodeTransformer* visitor) override;

        Token* a;
        Token* b;
    };

    struct AddAlias : public CallAlias
    {
        AddAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct SubtractAlias : public CallAlias
    {
        SubtractAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct MultiplyAlias : public CallAlias
    {
        MultiplyAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct DivideAlias : public CallAlias
    {
        DivideAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct PowerAlias : public CallAlias
    {
        PowerAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct EqualAlias : public CallAlias
    {
        EqualAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct LessAlias : public CallAlias
    {
        LessAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct GreaterAlias : public CallAlias
    {
        GreaterAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct LessEqualAlias : public CallAlias
    {
        LessEqualAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct GreaterEqualAlias : public CallAlias
    {
        GreaterEqualAlias(const SourceLocation location, Token* a, Token* b);

        void transform(BytecodeTransformer* visitor) const override;
    };

    struct Define : public Token
    {
        Define(const SourceLocation location, const std::string name, const std::vector<Identifier*> inputs, const std::vector<Token*> instructions);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;

        const std::string name;

        const std::vector<Identifier*> inputs;
        const std::vector<Token*> instructions;

        InstructionBlock* block;
    };

    struct Scope
    {
        Scope(const std::string name, const std::vector<Identifier*>& inputs);

        const std::string name;

        std::unordered_map<std::string, Identifier*> variables;
        std::unordered_set<std::string> variablesUsed;

        std::unordered_map<std::string, Identifier*> inputs;
        std::unordered_set<std::string> inputsUsed;

        std::unordered_map<std::string, Define*> functions;
        std::unordered_set<std::string> functionsUsed;
    };

    struct Program : public Token
    {
        Program(const SourceLocation location, const std::vector<Token*> instructions);

        const std::vector<Token*> instructions;
    };

    struct Include : public Token
    {
        Include(const SourceLocation location, Program* program);

        void resolveTypes(BytecodeTransformer* visitor) override;
        void transform(BytecodeTransformer* visitor) const override;

        Program* program;
    };

    struct BytecodeTransformer
    {
        BytecodeTransformer(const Path* sourcePath, std::ofstream& outputStream);

        void createBytecode(Program* program);

        void resolveTypes(Identifier* token);
        void resolveTypes(List* token);
        void resolveTypes(ParenthesizedExpression* token);
        void resolveTypes(Assign* token);
        void resolveTypes(Hold* token);
        void resolveTypes(LFO* token);
        void resolveTypes(Sweep* token);
        void resolveTypes(Sequence* token);
        void resolveTypes(Repeat* token);
        void resolveTypes(Random* token);
        void resolveTypes(Limit* token);
        void resolveTypes(Trigger* token);
        void resolveTypes(If* token);
        void resolveTypes(Sine* token);
        void resolveTypes(Square* token);
        void resolveTypes(Triangle* token);
        void resolveTypes(Saw* token);
        void resolveTypes(Noise* token);
        void resolveTypes(Sample* token);
        void resolveTypes(Delay* token);
        void resolveTypes(Perform* token);
        void resolveTypes(CallUser* token);
        void resolveTypes(CallAlias* token);
        void resolveTypes(Define* token);
        void resolveTypes(Include* token);

        void transform(const Value* token);
        void transform(const SequenceForwards* token);
        void transform(const SequenceBackwards* token);
        void transform(const SequencePingPong* token);
        void transform(const SequenceRandom* token);
        void transform(const RandomStep* token);
        void transform(const RandomLinear* token);
        void transform(const Pi* token);
        void transform(const E* token);
        void transform(const Identifier* token);
        void transform(const List* token);
        void transform(const ParenthesizedExpression* token);
        void transform(const Assign* token);
        void transform(const Time* token);
        void transform(const Hold* token);
        void transform(const LFO* token);
        void transform(const Sweep* token);
        void transform(const Sequence* token);
        void transform(const Repeat* token);
        void transform(const Random* token);
        void transform(const Limit* token);
        void transform(const Trigger* token);
        void transform(const If* token);
        void transform(const Sine* token);
        void transform(const Square* token);
        void transform(const Triangle* token);
        void transform(const Saw* token);
        void transform(const Noise* token);
        void transform(const Sample* token);
        void transform(const Delay* token);
        void transform(const Perform* token);
        void transform(const CallUser* token);
        void transform(const AddAlias* token);
        void transform(const SubtractAlias* token);
        void transform(const MultiplyAlias* token);
        void transform(const DivideAlias* token);
        void transform(const PowerAlias* token);
        void transform(const EqualAlias* token);
        void transform(const LessAlias* token);
        void transform(const GreaterAlias* token);
        void transform(const LessEqualAlias* token);
        void transform(const GreaterEqualAlias* token);
        void transform(const Define* token);
        void transform(const Include* token);

    private:
        void resolveArgumentTypes(const ArgumentList* arguments, const std::string name, Type* expectedType);
        void transformArgument(const ArgumentList* arguments, const std::string name);
        Token* getArgument(const ArgumentList* arguments, const std::string name) const;
        void checkArguments(const ArgumentList* arguments) const;

        void addInstruction(const BytecodeInstruction* instruction);

        Identifier* getVariable(const std::string variable);
        void addVariable(Identifier* variable);

        Identifier* getInput(const std::string input);

        Define* getFunction(const std::string function);
        void addFunction(Define* function);

        bool checkRecursive(const std::string function) const;
        void checkUses() const;

        const Path* sourcePath;

        std::ofstream& outputStream;

        std::unordered_map<const Path*, unsigned char, Path::Hash, Path::Equals> resources;

        BytecodeResolver* resolver = new BytecodeResolver();

        Utils* utils;

        std::vector<Scope*> scopes;

        Type* expectedType = new Type(BasicType::Any);

        std::stack<InstructionBlock*> blocks;

        unsigned char nextIdentifierId = 0;

    };
}
