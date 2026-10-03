#include "../include/types.h"

using namespace Parser;

FillContext::FillContext(FillContext* parent, const FillTypes& types) :
    parent(parent), types(types) {}

const SharedType FillContext::findType(const std::string& name) const
{
    if (types.count(name))
    {
        return types.at(name);
    }

    if (parent)
    {
        return parent->findType(name);
    }

    return SharedType(nullptr);
}

Type::Type(const TypeConstant& base, const std::string& singularStr, const std::string& pluralStr) :
    base(base), singularStr(singularStr), pluralStr(pluralStr) {}

Type::~Type() {}

TypeConstant Type::baseType() const
{
    return base;
}

std::string Type::singular() const
{
    return singularStr;
}

std::string Type::plural() const
{
    return pluralStr;
}

bool Type::checkType(const FillContext* context, const Type* actual) const
{
    if (base == actual->base)
    {
        return true;
    }

    if (actual->base != TypeConstant::Fillable)
    {
        return false;
    }

    if (const SharedType type = context->findType(dynamic_cast<const FillableType*>(actual)->input))
    {
        return base == type->base;
    }

    return false;
}

AnyType::AnyType() :
    Type(TypeConstant::Any, "anything", "anything") {}

bool AnyType::checkType(const FillContext* context, const Type* actual) const
{
    return actual->baseType() != TypeConstant::None;
}

NoneType::NoneType() :
    Type(TypeConstant::None, "nothing", "nothing") {}

SequenceOrderType::SequenceOrderType() :
    Type(TypeConstant::SequenceOrder, "a sequence order constant", "sequence order constants") {}

RandomTypeType::RandomTypeType() :
    Type(TypeConstant::RandomType, "a random type constant", "random type constants") {}

RoundDirectionType::RoundDirectionType() :
    Type(TypeConstant::RoundDirection, "a round direction constant", "round direction constants") {}

NumberType::NumberType() :
    Type(TypeConstant::Number, "a number", "numbers") {}

BooleanType::BooleanType() :
    Type(TypeConstant::Boolean, "a boolean", "booleans") {}

StringType::StringType() :
    Type(TypeConstant::String, "a string", "strings") {}

AudioSourceType::AudioSourceType() :
    Type(TypeConstant::AudioSource, "an audio source", "audio sources") {}

EffectType::EffectType() :
    Type(TypeConstant::Effect, "an effect", "effects") {}

ListType::ListType(const SharedType& subType) :
    Type(TypeConstant::List, "a list of " + subType->plural(), "lists of " + subType->plural()), subType(subType) {}

ListType::ListType(const Type* subType) :
    ListType(SharedType(subType)) {}

bool ListType::checkType(const FillContext* context, const Type* actual) const
{
    if (actual->baseType() != TypeConstant::List)
    {
        return false;
    }

    return subType->checkType(context, dynamic_cast<const ListType*>(actual)->subType.get());
}

FillableType::FillableType(const std::string& input) :
    Type(TypeConstant::Fillable, "a fillable", "fillables"), input(input) {}
