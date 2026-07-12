#include "../include/types.h"

using namespace Parser;

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

bool Type::checkType(const SharedType& actual) const
{
    return base == actual->base;
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

bool ListType::checkType(const SharedType& actual) const
{
    if (actual->baseType() != TypeConstant::List)
    {
        return false;
    }

    return subType->checkType(dynamic_cast<const ListType*>(actual.get())->subType);
}

LambdaType::LambdaType(const std::unordered_map<std::string, const SharedType>& inputTypes, const SharedType& returnType) :
    Type(TypeConstant::Lambda, getName(inputTypes, returnType)), inputTypes(inputTypes), returnType(returnType) {}

LambdaType::LambdaType(const std::unordered_map<std::string, const SharedType>& inputTypes, const Type* returnType) :
    LambdaType(inputTypes, SharedType(returnType)) {}

bool LambdaType::checkType(const SharedType& actual) const
{
    if (actual->baseType() != TypeConstant::Lambda)
    {
        return false;
    }

    const LambdaType* lambda = dynamic_cast<const LambdaType*>(actual.get());

    if (!returnType->checkType(lambda->returnType))
    {
        return false;
    }

    for (const std::pair<std::string, const SharedType>& input : inputTypes)
    {
        if (!lambda->inputTypes.count(input.first) || !input.second->checkType(lambda->inputTypes.at(input.first)))
        {
            return false;
        }
    }

    return true;
}

std::string LambdaType::getName(const std::unordered_map<std::string, const SharedType>& inputTypes, const SharedType& returnType)
{
    if (inputTypes.empty())
    {
        return "function with no inputs returning " + returnType->name();
    }

    std::string str;

    if (inputTypes.size() == 1)
    {
        str += "with input (";
    }

    else
    {
        str += "with inputs (";
    }

    bool first = true;

    for (const std::pair<std::string, const SharedType>& input : inputTypes)
    {
        if (first)
        {
            str += input.first + ": " + input.second->name();
        }

        else
        {
            str += ", " + input.first + ": " + input.second->name();
        }

        first = false;
    }

    return "function " + str + ") returning " + returnType->name();
}
