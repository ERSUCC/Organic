#pragma once

#include <string>

#include "constants.h"
#include "token.h"
#include "token_decls.h"
#include "types.h"

namespace Parser {

struct TypeResolver
{
    void resolveTypes(const VariableDef* token) const;
    void resolveTypes(const InputDef* token) const;
    void resolveTypes(const FunctionDef* token) const;
    void resolveTypes(const List* token) const;
    void resolveTypes(const ParenthesizedExpression* token) const;
    void resolveTypes(const Time* token) const;
    void resolveTypes(const Hold* token) const;
    void resolveTypes(const LFO* token) const;
    void resolveTypes(const Sweep* token) const;
    void resolveTypes(const Sequence* token) const;
    void resolveTypes(const Repeat* token) const;
    void resolveTypes(const Random* token) const;
    void resolveTypes(const Limit* token) const;
    void resolveTypes(const Trigger* token) const;
    void resolveTypes(const If* token) const;
    void resolveTypes(const All* token) const;
    void resolveTypes(const Any* token) const;
    void resolveTypes(const None* token) const;
    void resolveTypes(const Min* token) const;
    void resolveTypes(const Max* token) const;
    void resolveTypes(const Round* token) const;
    void resolveTypes(const Absolute* token) const;
    void resolveTypes(const Sine* token) const;
    void resolveTypes(const Square* token) const;
    void resolveTypes(const Triangle* token) const;
    void resolveTypes(const Saw* token) const;
    void resolveTypes(const Oscillator* token) const;
    void resolveTypes(const Noise* token) const;
    void resolveTypes(const Sample* token) const;
    void resolveTypes(const Granulate* token) const;
    void resolveTypes(const Group* token) const;
    void resolveTypes(const EffectGroup* token) const;
    void resolveTypes(const Delay* token) const;
    void resolveTypes(const Comb* token) const;
    void resolveTypes(const AllPass* token) const;
    void resolveTypes(const LowPass* token) const;
    void resolveTypes(const Reverb* token) const;
    void resolveTypes(const CallUser* token) const;
    void resolveTypes(const CallAlias* token) const;
    void resolveTypes(const Program* token) const;

private:
    void resolveArgumentTypes(ArgumentList* arguments, const std::string name, const SharedType expectedType, const SharedToken defaultValue) const;
    void resolveArgumentTypes(ArgumentList* arguments, const std::string name, const Type* expectedType, const Token* defaultValue = nullptr) const;

};

}
