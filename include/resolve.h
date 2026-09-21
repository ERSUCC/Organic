#pragma once

#include <string>

#include "constants.h"
#include "token.h"
#include "token_decls.h"
#include "types.h"

namespace Parser {

struct TypeResolver
{
    ~TypeResolver();

    void resolveTypes(const VariableDef* token);
    void resolveTypes(const InputDef* token);
    void resolveTypes(const FunctionDef* token);
    void resolveTypes(const List* token);
    void resolveTypes(const ParenthesizedExpression* token);
    void resolveTypes(const Negate* token);
    void resolveTypes(const Time* token);
    void resolveTypes(const Hold* token);
    void resolveTypes(const LFO* token);
    void resolveTypes(const Sweep* token);
    void resolveTypes(const Sequence* token);
    void resolveTypes(const Repeat* token);
    void resolveTypes(const Random* token);
    void resolveTypes(const Limit* token);
    void resolveTypes(const Trigger* token);
    void resolveTypes(const If* token);
    void resolveTypes(const All* token);
    void resolveTypes(const Any* token);
    void resolveTypes(const None* token);
    void resolveTypes(const Min* token);
    void resolveTypes(const Max* token);
    void resolveTypes(const Round* token);
    void resolveTypes(const Absolute* token);
    void resolveTypes(const Modulo* token);
    void resolveTypes(const Logarithm* token);
    void resolveTypes(const Sine* token);
    void resolveTypes(const Square* token);
    void resolveTypes(const Triangle* token);
    void resolveTypes(const Saw* token);
    void resolveTypes(const Oscillator* token);
    void resolveTypes(const Noise* token);
    void resolveTypes(const Sample* token);
    void resolveTypes(const Granulate* token);
    void resolveTypes(const Group* token);
    void resolveTypes(const EffectGroup* token);
    void resolveTypes(const Delay* token);
    void resolveTypes(const Comb* token);
    void resolveTypes(const AllPass* token);
    void resolveTypes(const LowPass* token);
    void resolveTypes(const Reverb* token);
    void resolveTypes(const CallUser* token);
    void resolveTypes(const CallAlias* token);
    void resolveTypes(const Program* token);

private:
    void resolveArgumentTypes(ArgumentList* arguments, const std::string& name, const SharedType& expectedType, const SharedToken& defaultValue, const FillTypes& types);
    void resolveArgumentTypes(ArgumentList* arguments, const std::string& name, const Type* expectedType, const Token* defaultValue = nullptr, const FillTypes& types = {});

    FillContext* context = new FillContext(nullptr, {});

};

}
