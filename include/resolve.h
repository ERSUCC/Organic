#pragma once

#include <string>

#include "constants.h"
#include "token.h"
#include "token_decls.h"
#include "types.h"

namespace Parser {

struct TypeResolver
{
    static void resolveTypes(const VariableDef* token);
    static void resolveTypes(const InputDef* token);
    static void resolveTypes(const FunctionDef* token);
    static void resolveTypes(const List* token);
    static void resolveTypes(const ParenthesizedExpression* token);
    static void resolveTypes(const Negate* token);
    static void resolveTypes(const Time* token);
    static void resolveTypes(const Hold* token);
    static void resolveTypes(const LFO* token);
    static void resolveTypes(const Sweep* token);
    static void resolveTypes(const Sequence* token);
    static void resolveTypes(const Repeat* token);
    static void resolveTypes(const Random* token);
    static void resolveTypes(const Limit* token);
    static void resolveTypes(const Trigger* token);
    static void resolveTypes(const If* token);
    static void resolveTypes(const All* token);
    static void resolveTypes(const Any* token);
    static void resolveTypes(const None* token);
    static void resolveTypes(const Min* token);
    static void resolveTypes(const Max* token);
    static void resolveTypes(const Round* token);
    static void resolveTypes(const Absolute* token);
    static void resolveTypes(const Sine* token);
    static void resolveTypes(const Square* token);
    static void resolveTypes(const Triangle* token);
    static void resolveTypes(const Saw* token);
    static void resolveTypes(const Oscillator* token);
    static void resolveTypes(const Noise* token);
    static void resolveTypes(const Sample* token);
    static void resolveTypes(const Granulate* token);
    static void resolveTypes(const Group* token);
    static void resolveTypes(const EffectGroup* token);
    static void resolveTypes(const Delay* token);
    static void resolveTypes(const Comb* token);
    static void resolveTypes(const AllPass* token);
    static void resolveTypes(const LowPass* token);
    static void resolveTypes(const Reverb* token);
    static void resolveTypes(const CallUser* token);
    static void resolveTypes(const CallAlias* token);
    static void resolveTypes(const Program* token);

private:
    static void resolveArgumentTypes(ArgumentList* arguments, const std::string& name, const SharedType& expectedType, const SharedToken& defaultValue);
    static void resolveArgumentTypes(ArgumentList* arguments, const std::string& name, const Type* expectedType, const Token* defaultValue = nullptr);

};

}
