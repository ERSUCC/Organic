#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "location.h"

namespace Parser {

struct Type;

typedef std::unique_ptr<const Type> UniqueType;
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
    Fillable
};

typedef std::unordered_map<std::string, SharedType> FillTypes;

struct FillContext
{
    FillContext(FillContext* parent, const FillTypes& types);

    const SharedType findType(const std::string& name) const;

    FillContext* parent;

private:
    const FillTypes types;

};

struct Type
{
    Type(const TypeConstant& base, const std::string& str);

    virtual ~Type();

    TypeConstant baseType() const;

    std::string name() const;

    virtual bool checkType(const FillContext* context, const Type* actual) const;

private:
    const TypeConstant base;

    const std::string str;

};

struct AnyType : public Type
{
    AnyType();

    bool checkType(const FillContext* context, const Type* actual) const override;
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

    bool checkType(const FillContext* context, const Type* actual) const override;

    const SharedType subType;
};

struct FillableType : public Type
{
    FillableType(const std::string& input);

    const std::string input;
};

}
