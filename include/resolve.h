#pragma once

#include <string>

#include "constants.h"
#include "token.h"
#include "types.h"

namespace Parser {

struct TypeResolver
{
    void resolveTypes(InputDef* token);
    void resolveTypes(List* token);
    void resolveTypes(ParenthesizedExpression* token);
    void resolveTypes(Assign* token);
    void resolveTypes(Time* token);
    void resolveTypes(Hold* token);
    void resolveTypes(LFO* token);
    void resolveTypes(Sweep* token);
    void resolveTypes(Sequence* token);
    void resolveTypes(Repeat* token);
    void resolveTypes(Random* token);
    void resolveTypes(Limit* token);
    void resolveTypes(Trigger* token);
    void resolveTypes(If* token);
    void resolveTypes(All* token);
    void resolveTypes(Any* token);
    void resolveTypes(None* token);
    void resolveTypes(Min* token);
    void resolveTypes(Max* token);
    void resolveTypes(Round* token);
    void resolveTypes(Absolute* token);
    void resolveTypes(Sine* token);
    void resolveTypes(Square* token);
    void resolveTypes(Triangle* token);
    void resolveTypes(Saw* token);
    void resolveTypes(Oscillator* token);
    void resolveTypes(Noise* token);
    void resolveTypes(Sample* token);
    void resolveTypes(Granulate* token);
    void resolveTypes(Group* token);
    void resolveTypes(EffectGroup* token);
    void resolveTypes(Delay* token);
    void resolveTypes(Comb* token);
    void resolveTypes(AllPass* token);
    void resolveTypes(LowPass* token);
    void resolveTypes(Reverb* token);
    void resolveTypes(CallUser* token);
    void resolveTypes(CallAlias* token);
    void resolveTypes(Define* token);
    void resolveTypes(Program* token);

private:
    void resolveArgumentTypes(ArgumentList* arguments, const std::string name, const SharedType expectedType, Token* defaultValue = nullptr);
    void resolveArgumentTypes(ArgumentList* arguments, const std::string name, const Type* expectedType, Token* defaultValue = nullptr);

};

}
