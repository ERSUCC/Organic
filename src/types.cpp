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

Type::Type(const TypeConstant& base, const std::string& str) :
    base(base), str(str) {}

Type::~Type() {}

TypeConstant Type::baseType() const
{
    return base;
}

std::string Type::name() const
{
    return str;
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
    Type(TypeConstant::Any, "anything") {}

bool AnyType::checkType(const FillContext* context, const Type* actual) const
{
    return actual->baseType() != TypeConstant::None;
}

NoneType::NoneType() :
    Type(TypeConstant::None, "nothing") {}

SequenceOrderType::SequenceOrderType() :
    Type(TypeConstant::SequenceOrder, "sequence order constant") {}

RandomTypeType::RandomTypeType() :
    Type(TypeConstant::RandomType, "random type constant") {}

RoundDirectionType::RoundDirectionType() :
    Type(TypeConstant::RoundDirection, "round direction constant") {}

NumberType::NumberType() :
    Type(TypeConstant::Number, "number") {}

BooleanType::BooleanType() :
    Type(TypeConstant::Boolean, "boolean") {}

StringType::StringType() :
    Type(TypeConstant::String, "string") {}

AudioSourceType::AudioSourceType() :
    Type(TypeConstant::AudioSource, "audio source") {}

EffectType::EffectType() :
    Type(TypeConstant::Effect, "effect") {}

ListType::ListType(const SharedType& subType) :
    Type(TypeConstant::List, "list of " + subType->name()), subType(subType) {}

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
    Type(TypeConstant::Fillable, "fillable"), input(input) {}
