#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "exception.h"
#include "constants.h"
#include "location.h"
#include "path.h"
#include "program.h"
#include "resolve.h"
#include "token_decls.h"
#include "transform.h"
#include "types.h"
#include "utils.h"

struct TokenTransformer;

namespace Parser {

struct TypeResolver;

struct Token
{
    Token(const SourceLocation& location, const Type* type = new NoneType());

    virtual ~Token();

    virtual const SharedType type() const;

    inline const std::string string() const
    {
        return location.string();
    }

    inline const SourceProvider* source() const
    {
        return location.source;
    }

    inline size_t start() const
    {
        return location.start;
    }

    inline size_t end() const
    {
        return location.end;
    }

    virtual bool eof() const;

    virtual void resolveTypes(const TypeResolver* visitor) const;

    virtual Engine::ValueObject* transform(TokenTransformer* visitor) const;

    const SourceLocation location;

private:
    const SharedType staticType;

};

struct Eof : public Token
{
    Eof(const SourceLocation& location);

    bool eof() const override;
};

struct OpenParenthesis : public Token
{
    OpenParenthesis(const SourceLocation& location);
};

struct CloseParenthesis : public Token
{
    CloseParenthesis(const SourceLocation& location);
};

struct OpenSquareBracket : public Token
{
    OpenSquareBracket(const SourceLocation& location);
};

struct CloseSquareBracket : public Token
{
    CloseSquareBracket(const SourceLocation& location);
};

struct OpenCurlyBracket : public Token
{
    OpenCurlyBracket(const SourceLocation& location);
};

struct CloseCurlyBracket : public Token
{
    CloseCurlyBracket(const SourceLocation& location);
};

struct Colon : public Token
{
    Colon(const SourceLocation& location);
};

struct Comma : public Token
{
    Comma(const SourceLocation& location);
};

struct Equals : public Token
{
    Equals(const SourceLocation& location);
};

struct Operator : public Token
{
    Operator(const SourceLocation& location);
};

struct Add : public Operator
{
    Add(const SourceLocation& location);
};

struct Subtract : public Operator
{
    Subtract(const SourceLocation& location);
};

struct Multiply : public Operator
{
    Multiply(const SourceLocation& location);
};

struct Divide : public Operator
{
    Divide(const SourceLocation& location);
};

struct Power : public Operator
{
    Power(const SourceLocation& location);
};

struct BooleanOperator : public Operator
{
    BooleanOperator(const SourceLocation& location);
};

struct DoubleEquals : public BooleanOperator
{
    DoubleEquals(const SourceLocation& location);
};

struct Less : public BooleanOperator
{
    Less(const SourceLocation& location);
};

struct Greater : public BooleanOperator
{
    Greater(const SourceLocation& location);
};

struct LessEqual : public BooleanOperator
{
    LessEqual(const SourceLocation& location);
};

struct GreaterEqual : public BooleanOperator
{
    GreaterEqual(const SourceLocation& location);
};

struct Identifier : public Token
{
    Identifier(const SourceLocation& location);
};

struct EmptyLambda : public Token
{
    EmptyLambda(const SourceLocation& location, const Token* value);
    ~EmptyLambda();

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const Token* value;
};

struct Value : public Token
{
    Value(const SourceLocation& location, const double value);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const double value;
};

struct Constant : public Token
{
    Constant(const SourceLocation& location, const Type* type, const unsigned char value);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const unsigned char value;
};

struct String : public Token
{
    String(const SourceLocation& location, const std::string& str);

    const std::string str;
};

struct VariableDef : public Identifier
{
    VariableDef(const SourceLocation& location, const Token* value);
    ~VariableDef();

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const Token* value;
};

struct VariableRef : public Identifier
{
    VariableRef(const SourceLocation& location, const VariableDef* definition);

    const SharedType type() const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const VariableDef* definition;
};

struct InputDef : public Identifier
{
    InputDef(const SourceLocation& location, const SharedToken& defaultValue);

    const SharedType type() const override;

    void resolveTypes(const TypeResolver* visitor) const override;

    const SharedToken defaultValue;
};

struct InputRef : public Identifier
{
    InputRef(const SourceLocation& location, const InputDef* definition);

    const SharedType type() const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const InputDef* definition;
};

struct FunctionDef : public Identifier
{
    FunctionDef(const SourceLocation& location, const std::vector<const InputDef*>& inputs, const Program* program);
    ~FunctionDef();

    const SharedType returnType() const;

    void resolveTypes(const TypeResolver* visitor) const override;

    const std::vector<const InputDef*> inputs;

    const Program* program;
};

struct FunctionRef : public Identifier
{
    FunctionRef(const SourceLocation& location, const FunctionDef* definition);

    const SharedType type() const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const FunctionDef* definition;
};

struct Argument : public Token
{
    Argument(const SourceLocation& location, const std::string& name, const SharedToken& value);

    const std::string name;

    const SharedToken value;
};

struct ArgumentList : public Token
{
    ArgumentList(const SourceLocation& location, const std::vector<const Argument*>& arguments, const std::string& name);
    ~ArgumentList();

    const Argument* findArgument(const std::string& name);

    void addDefault(const std::string& name, const SharedToken& value);

    void check() const;

    std::vector<const Argument*> arguments;

    std::unordered_set<const Argument*> used;

    const std::string name;
};

struct List : public Token
{
    List(const SourceLocation& location, const std::vector<const Token*>& values);
    ~List();

    const SharedType type() const override;

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const std::vector<const Token*> values;
};

struct ParenthesizedExpression : public Token
{
    ParenthesizedExpression(const SourceLocation& location, const Token* value);
    ~ParenthesizedExpression();

    const SharedType type() const override;

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const Token* value;
};

struct Negate : public Token
{
    Negate(const SourceLocation& location, const Token* value);
    ~Negate();

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const Token* value;
};

struct Call : public Token
{
    Call(const SourceLocation& location, ArgumentList* arguments, const Type* type = new NoneType());
    ~Call();

    ArgumentList* arguments;
};

struct Time : public Call
{
    Time(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Hold : public Call
{
    Hold(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct LFO : public Call
{
    LFO(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Sweep : public Call
{
    Sweep(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Sequence : public Call
{
    Sequence(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Repeat : public Call
{
    Repeat(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Random : public Call
{
    Random(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Limit : public Call
{
    Limit(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Trigger : public Call
{
    Trigger(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct If : public Call
{
    If(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct All : public Call
{
    All(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Any : public Call
{
    Any(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct None : public Call
{
    None(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Min : public Call
{
    Min(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Max : public Call
{
    Max(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Round : public Call
{
    Round(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Absolute : public Call
{
    Absolute(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct AudioSource : public Call
{
    AudioSource(const SourceLocation& location, ArgumentList* arguments);
};

struct EmptyAudioSource : public AudioSource
{
    EmptyAudioSource(const SourceLocation& location);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Sine : public AudioSource
{
    Sine(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Square : public AudioSource
{
    Square(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Triangle : public AudioSource
{
    Triangle(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Saw : public AudioSource
{
    Saw(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Oscillator : public AudioSource
{
    Oscillator(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Noise : public AudioSource
{
    Noise(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Sample : public AudioSource
{
    Sample(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Granulate : public AudioSource
{
    Granulate(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Group : public AudioSource
{
    Group(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Effect : public Call
{
    Effect(const SourceLocation& location, ArgumentList* arguments);
};

struct EmptyEffect : public Effect
{
    EmptyEffect(const SourceLocation& location);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct EffectGroup : public Effect
{
    EffectGroup(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Delay : public Effect
{
    Delay(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Comb : public Effect
{
    Comb(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct AllPass : public Effect
{
    AllPass(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct LowPass : public Effect
{
    LowPass(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Reverb : public Effect
{
    Reverb(const SourceLocation& location, ArgumentList* arguments);

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct CallUser : public Call
{
    CallUser(const SourceLocation& location, ArgumentList* arguments, const FunctionDef* function);

    const SharedType type() const override;

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const FunctionDef* function;
};

struct CallAlias : public Call
{
    CallAlias(const Token* a, const Token* b, const std::string& op, const Type* type);

    void resolveTypes(const TypeResolver* visitor) const override;

private:
    const std::string op;

};

struct AddAlias : public CallAlias
{
    AddAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct SubtractAlias : public CallAlias
{
    SubtractAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct MultiplyAlias : public CallAlias
{
    MultiplyAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct DivideAlias : public CallAlias
{
    DivideAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct PowerAlias : public CallAlias
{
    PowerAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct EqualAlias : public CallAlias
{
    EqualAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct LessAlias : public CallAlias
{
    LessAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct GreaterAlias : public CallAlias
{
    GreaterAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct LessEqualAlias : public CallAlias
{
    LessEqualAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct GreaterEqualAlias : public CallAlias
{
    GreaterEqualAlias(const Token* a, const Token* b);

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;
};

struct Program : public Token
{
    Program(const SourceLocation& location, const std::vector<const Token*>& instructions);
    ~Program();

    void resolveTypes(const TypeResolver* visitor) const override;

    Engine::ValueObject* transform(TokenTransformer* visitor) const override;

    const std::vector<const Token*> instructions;
};

}
