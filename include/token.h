#pragma once

#include <string>
#include <vector>

#include "exception.h"
#include "constants.h"
#include "location.h"
#include "path.h"
#include "program.h"
#include "transform.h"
#include "types.h"
#include "utils.h"

struct TokenTransformer;

namespace Parser {

struct Type;
struct TypeResolver;

struct Token
{
    Token(const SourceLocation location);

    virtual void resolveTypes(TypeResolver* visitor);

    virtual Engine::ValueObject* transform(TokenTransformer* visitor) const;

    const std::string string() const;

    const SourceLocation location;

    Type* type = nullptr;
};

struct OpenParenthesis : public Token
{
    OpenParenthesis(const SourceLocation location);
};

struct CloseParenthesis : public Token
{
    CloseParenthesis(const SourceLocation location);
};

struct OpenSquareBracket : public Token
{
    OpenSquareBracket(const SourceLocation location);
};

struct CloseSquareBracket : public Token
{
    CloseSquareBracket(const SourceLocation location);
};

struct OpenCurlyBracket : public Token
{
    OpenCurlyBracket(const SourceLocation location);
};

struct CloseCurlyBracket : public Token
{
    CloseCurlyBracket(const SourceLocation location);
};

struct Colon : public Token
{
    Colon(const SourceLocation location);
};

struct Comma : public Token
{
    Comma(const SourceLocation location);
};

struct Equals : public Token
{
    Equals(const SourceLocation location);
};

struct Operator : public Token
{
    Operator(const SourceLocation location);
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

struct Identifier : public Token
{
    Identifier(const SourceLocation location);
};

struct EmptyLambda : public Token
{
    EmptyLambda(const SourceLocation location, Token* value);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    Token* value;
};

struct Value : public Token
{
    Value(const SourceLocation location, const double value);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const double value;
};

struct Constant : public Token
{
    Constant(const SourceLocation location, const unsigned char value);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const unsigned char value;
};

struct SequenceForward : public Constant
{
    SequenceForward(const SourceLocation location);
};

struct SequenceBackward : public Constant
{
    SequenceBackward(const SourceLocation location);
};

struct SequencePingPong : public Constant
{
    SequencePingPong(const SourceLocation location);
};

struct SequenceRandom : public Constant
{
    SequenceRandom(const SourceLocation location);
};

struct RandomStep : public Constant
{
    RandomStep(const SourceLocation location);
};

struct RandomLinear : public Constant
{
    RandomLinear(const SourceLocation location);
};

struct RoundNearest : public Constant
{
    RoundNearest(const SourceLocation location);
};

struct RoundUp : public Constant
{
    RoundUp(const SourceLocation location);
};

struct RoundDown : public Constant
{
    RoundDown(const SourceLocation location);
};

struct String : public Token
{
    String(const SourceLocation location, const std::string str);

    const std::string str;
};

struct VariableDef : public Identifier
{
    VariableDef(const SourceLocation location);

    bool used = false;
};

struct VariableRef : public Identifier
{
    VariableRef(const SourceLocation location, VariableDef* definition);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    VariableDef* definition;
};

struct InputDef : public Identifier
{
    InputDef(const SourceLocation location, Token* defaultValue);

    void resolveTypes(TypeResolver* visitor) override;

    Token* defaultValue;

    bool used = false;
};

struct InputRef : public Identifier
{
    InputRef(const SourceLocation location, InputDef* definition);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    InputDef* definition;
};

struct FunctionDef : public Identifier
{
    FunctionDef(const SourceLocation location, const std::vector<InputDef*>& inputs);

    const std::vector<InputDef*> inputs;

    Type* returnType;

    bool used = false;
};

struct FunctionRef : public Identifier
{
    FunctionRef(const SourceLocation location, FunctionDef* definition);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    FunctionDef* definition;
};

struct Argument : public Token
{
    Argument(const SourceLocation location, const std::string name, Token* value);

    const std::string name;

    Token* value;

    bool used = false;
};

struct ArgumentList : public Token
{
    ArgumentList(const SourceLocation location, const std::vector<Argument*>& arguments, const std::string name);

    Token* get(const std::string name) const;

    void addDefault(const std::string name, Token* value);

    void check() const;

    std::vector<Argument*> arguments;

    const std::string name;
};

struct List : public Token
{
    List(const SourceLocation location, const std::vector<Token*> values);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const std::vector<Token*> values;
};

struct ParenthesizedExpression : public Token
{
    ParenthesizedExpression(const SourceLocation location, Token* value);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    Token* value;
};

struct Assign : public Token
{
    Assign(const SourceLocation location, VariableDef* variable, Token* value);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    VariableDef* variable;

    Token* value;
};

struct Call : public Token
{
    Call(const SourceLocation location, ArgumentList* arguments);

    ArgumentList* arguments;
};

struct Time : public Call
{
    Time(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Hold : public Call
{
    Hold(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct LFO : public Call
{
    LFO(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Sweep : public Call
{
    Sweep(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Sequence : public Call
{
    Sequence(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Repeat : public Call
{
    Repeat(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Random : public Call
{
    Random(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Limit : public Call
{
    Limit(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Trigger : public Call
{
    Trigger(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct If : public Call
{
    If(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct All : public Call
{
    All(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Any : public Call
{
    Any(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct None : public Call
{
    None(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Min : public Call
{
    Min(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Max : public Call
{
    Max(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Round : public Call
{
    Round(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Absolute : public Call
{
    Absolute(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct AudioSource : public Call
{
    AudioSource(const SourceLocation location, ArgumentList* arguments);
};

struct EmptyAudioSource : public AudioSource
{
    EmptyAudioSource(const SourceLocation location);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Sine : public AudioSource
{
    Sine(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Square : public AudioSource
{
    Square(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Triangle : public AudioSource
{
    Triangle(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Saw : public AudioSource
{
    Saw(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Oscillator : public AudioSource
{
    Oscillator(const SourceLocation, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Noise : public AudioSource
{
    Noise(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Sample : public AudioSource
{
    Sample(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Granulate : public AudioSource
{
    Granulate(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Group : public AudioSource
{
    Group(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Effect : public Call
{
    Effect(const SourceLocation location, ArgumentList* arguments);
};

struct EmptyEffect : public Effect
{
    EmptyEffect(const SourceLocation location);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct EffectGroup : public Effect
{
    EffectGroup(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Delay : public Effect
{
    Delay(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Comb : public Effect
{
    Comb(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct AllPass : public Effect
{
    AllPass(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct LowPass : public Effect
{
    LowPass(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Reverb : public Effect
{
    Reverb(const SourceLocation location, ArgumentList* arguments);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct CallUser : public Call
{
    CallUser(const SourceLocation location, ArgumentList* arguments, FunctionRef* function);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    FunctionRef* function;
};

struct CallAlias : public Call
{
    CallAlias(const SourceLocation location, Token* a, Token* b, const std::string op);

    void resolveTypes(TypeResolver* visitor) override;

private:
    const std::string op;

};

struct AddAlias : public CallAlias
{
    AddAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct SubtractAlias : public CallAlias
{
    SubtractAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct MultiplyAlias : public CallAlias
{
    MultiplyAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct DivideAlias : public CallAlias
{
    DivideAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct PowerAlias : public CallAlias
{
    PowerAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct EqualAlias : public CallAlias
{
    EqualAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct LessAlias : public CallAlias
{
    LessAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct GreaterAlias : public CallAlias
{
    GreaterAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct LessEqualAlias : public CallAlias
{
    LessEqualAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct GreaterEqualAlias : public CallAlias
{
    GreaterEqualAlias(const SourceLocation location, Token* a, Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Define : public Token
{
    Define(const SourceLocation location, const std::vector<Token*> instructions, FunctionDef* function);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const std::vector<Token*> instructions;

    FunctionDef* function;
};

struct Program : public Token
{
    Program(const SourceLocation location, const std::vector<Token*> instructions);

    void resolveTypes(TypeResolver* visitor) override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const std::vector<Token*> instructions;
};

}
