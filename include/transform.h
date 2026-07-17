#pragma once

#include <stddef.h>
#include <string>
#include <vector>
#include <unordered_map>

#include "controller.h"
#include "object.h"
#include "path.h"
#include "program.h"
#include "resource.h"
#include "token.h"
#include "token_decls.h"
#include "utils.h"

struct TokenTransformer
{
    TokenTransformer(const Path& sourcePath);

    Engine::ValueObject* transform(const Parser::Value* token);
    Engine::ValueObject* transform(const Parser::Constant* token);
    Engine::ValueObject* transform(const Parser::Boolean* token);
    Engine::ValueObject* transform(const Parser::VariableDef* token);
    Engine::ValueObject* transform(const Parser::VariableRef* token);
    Engine::ValueObject* transform(const Parser::InputRef* token);
    Engine::ValueObject* transform(const Parser::FunctionRef* token);
    Engine::ValueObject* transform(const Parser::EmptyLambda* token);
    Engine::ValueObject* transform(const Parser::List* token);
    Engine::ValueObject* transform(const Parser::ParenthesizedExpression* token);
    Engine::ValueObject* transform(const Parser::Negate* token);
    Engine::ValueObject* transform(const Parser::Time* token);
    Engine::ValueObject* transform(const Parser::Hold* token);
    Engine::ValueObject* transform(const Parser::LFO* token);
    Engine::ValueObject* transform(const Parser::Sweep* token);
    Engine::ValueObject* transform(const Parser::Sequence* token);
    Engine::ValueObject* transform(const Parser::Repeat* token);
    Engine::ValueObject* transform(const Parser::Random* token);
    Engine::ValueObject* transform(const Parser::Limit* token);
    Engine::ValueObject* transform(const Parser::Trigger* token);
    Engine::ValueObject* transform(const Parser::If* token);
    Engine::ValueObject* transform(const Parser::All* token);
    Engine::ValueObject* transform(const Parser::Any* token);
    Engine::ValueObject* transform(const Parser::None* token);
    Engine::ValueObject* transform(const Parser::Min* token);
    Engine::ValueObject* transform(const Parser::Max* token);
    Engine::ValueObject* transform(const Parser::Round* token);
    Engine::ValueObject* transform(const Parser::Absolute* token);
    Engine::ValueObject* transform(const Parser::EmptyAudioSource* token);
    Engine::ValueObject* transform(const Parser::Sine* token);
    Engine::ValueObject* transform(const Parser::Square* token);
    Engine::ValueObject* transform(const Parser::Triangle* token);
    Engine::ValueObject* transform(const Parser::Saw* token);
    Engine::ValueObject* transform(const Parser::Oscillator* token);
    Engine::ValueObject* transform(const Parser::Noise* token);
    Engine::ValueObject* transform(const Parser::Sample* token);
    Engine::ValueObject* transform(const Parser::Granulate* token);
    Engine::ValueObject* transform(const Parser::Group* token);
    Engine::ValueObject* transform(const Parser::EmptyEffect* token);
    Engine::ValueObject* transform(const Parser::EffectGroup* token);
    Engine::ValueObject* transform(const Parser::Delay* token);
    Engine::ValueObject* transform(const Parser::Comb* token);
    Engine::ValueObject* transform(const Parser::AllPass* token);
    Engine::ValueObject* transform(const Parser::LowPass* token);
    Engine::ValueObject* transform(const Parser::Reverb* token);
    Engine::ValueObject* transform(const Parser::CallUser* token);
    Engine::ValueObject* transform(const Parser::AddAlias* token);
    Engine::ValueObject* transform(const Parser::SubtractAlias* token);
    Engine::ValueObject* transform(const Parser::MultiplyAlias* token);
    Engine::ValueObject* transform(const Parser::DivideAlias* token);
    Engine::ValueObject* transform(const Parser::PowerAlias* token);
    Engine::ValueObject* transform(const Parser::EqualAlias* token);
    Engine::ValueObject* transform(const Parser::LessAlias* token);
    Engine::ValueObject* transform(const Parser::GreaterAlias* token);
    Engine::ValueObject* transform(const Parser::LessEqualAlias* token);
    Engine::ValueObject* transform(const Parser::GreaterEqualAlias* token);
    Engine::ValueObject* transform(const Parser::Program* token);

private:
    Engine::ValueObject* transformArgument(const Parser::ArgumentList* arguments, const std::string& name);

    void setVariable(const Parser::Identifier* name, Engine::ValueObject* value);

    const Path sourcePath;

    std::unordered_map<const Parser::Identifier*, Engine::ValueObject*> currentVariables;

    std::vector<Engine::ValueObject*> allVariables;

};
