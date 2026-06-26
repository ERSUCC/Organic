#pragma once

#include <memory>

namespace Parser {

struct Token;

typedef std::shared_ptr<const Token> SharedToken;

struct Value;
struct Constant;
struct VariableDef;
struct VariableRef;
struct InputDef;
struct InputRef;
struct FunctionDef;
struct FunctionRef;
struct EmptyLambda;
struct ArgumentList;
struct List;
struct ParenthesizedExpression;
struct Time;
struct Hold;
struct LFO;
struct Sweep;
struct Sequence;
struct Repeat;
struct Random;
struct Limit;
struct Trigger;
struct If;
struct All;
struct Any;
struct None;
struct Min;
struct Max;
struct Round;
struct Absolute;
struct EmptyAudioSource;
struct Sine;
struct Square;
struct Triangle;
struct Saw;
struct Oscillator;
struct Noise;
struct Sample;
struct Granulate;
struct Group;
struct EmptyEffect;
struct EffectGroup;
struct Delay;
struct Comb;
struct AllPass;
struct LowPass;
struct Reverb;
struct CallUser;
struct CallAlias;
struct AddAlias;
struct SubtractAlias;
struct MultiplyAlias;
struct DivideAlias;
struct PowerAlias;
struct EqualAlias;
struct LessAlias;
struct GreaterAlias;
struct LessEqualAlias;
struct GreaterEqualAlias;
struct Program;

}
