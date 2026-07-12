#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "location.h"

namespace Parser {

struct Type;
struct NoneType;
struct SequenceOrderType;
struct RandomTypeType;
struct RoundDirectionType;
struct NumberType;
struct BooleanType;
struct StringType;
struct AudioSourceType;
struct EffectType;
struct ListType;
struct LambdaType;

typedef std::shared_ptr<const Type> SharedType;

enum struct TypeConstant
{
    None,
    Any,
    SequenceOrder,
    RandomType,
    RoundDirection,
    Number,
    Boolean,
    String,
    AudioSource,
    Effect,
    List,
    Lambda
};

struct Type
{
    Type(const TypeConstant& base, const std::string& str);

    virtual ~Type();

    TypeConstant baseType() const;

    std::string name() const;

    virtual bool checkType(const SharedType& actual) const;

private:
    const TypeConstant base;

    const std::string str;

};

struct NoneType : public Type
{
    NoneType();
};

struct SequenceOrderType : public Type
{
    SequenceOrderType();
};

struct RandomTypeType : public Type
{
    RandomTypeType();
};

struct RoundDirectionType : public Type
{
    RoundDirectionType();
};

struct NumberType : public Type
{
    NumberType();
};

struct BooleanType : public Type
{
    BooleanType();
};

struct StringType : public Type
{
    StringType();
};

struct AudioSourceType : public Type
{
    AudioSourceType();
};

struct EffectType : public Type
{
    EffectType();
};

struct ListType : public Type
{
    ListType(const SharedType& subType);
    ListType(const Type* subType);

    bool checkType(const SharedType& actual) const override;

private:
    const SharedType subType;

};

struct LambdaType : public Type
{
    LambdaType(const std::unordered_map<std::string, const SharedType>& inputTypes, const SharedType& returnType);
    LambdaType(const std::unordered_map<std::string, const SharedType>& inputTypes, const Type* returnType);

    bool checkType(const SharedType& actual) const override;

private:
    static std::string getName(const std::unordered_map<std::string, const SharedType>& inputTypes, const SharedType& returnType);

    const std::unordered_map<std::string, const SharedType> inputTypes;

    const SharedType returnType;

};

}
