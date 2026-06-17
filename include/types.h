#pragma once

#include <string>
#include <unordered_map>
#include <utility>

#include "location.h"
#include "path.h"
#include "token.h"
#include "token_decls.h"

namespace Parser {

struct NoneType;
struct AnyType;
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

struct Type
{
    Type(const std::string str);

    std::string name() const;

    virtual bool checkType(const Type* actual) const = 0;

    virtual bool checkSpecifiedType(const NoneType* expected) const;
    virtual bool checkSpecifiedType(const AnyType* expected) const;
    virtual bool checkSpecifiedType(const SequenceOrderType* expected) const;
    virtual bool checkSpecifiedType(const RandomTypeType* expected) const;
    virtual bool checkSpecifiedType(const RoundDirectionType* expected) const;
    virtual bool checkSpecifiedType(const NumberType* expected) const;
    virtual bool checkSpecifiedType(const BooleanType* expected) const;
    virtual bool checkSpecifiedType(const StringType* expected) const;
    virtual bool checkSpecifiedType(const AudioSourceType* expected) const;
    virtual bool checkSpecifiedType(const EffectType* expected) const;
    virtual bool checkSpecifiedType(const ListType* expected) const;
    virtual bool checkSpecifiedType(const LambdaType* expected) const;

    virtual Token* getDefault(const SourceLocation location) const;

private:
    const std::string str;

};

struct NoneType : public Type
{
    NoneType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const NoneType* expected) const override;
};

struct AnyType : public Type
{
    AnyType();

    bool checkType(const Type* actual) const override;
};

struct SequenceOrderType : public Type
{
    SequenceOrderType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const SequenceOrderType* expected) const override;

    Token* getDefault(const SourceLocation location) const override;
};

struct RandomTypeType : public Type
{
    RandomTypeType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const RandomTypeType* expected) const override;

    Token* getDefault(const SourceLocation location) const override;
};

struct RoundDirectionType : public Type
{
    RoundDirectionType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const RoundDirectionType* expected) const override;

    Token* getDefault(const SourceLocation location) const override;
};

struct NumberType : public Type
{
    NumberType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const NumberType* expected) const override;
};

struct BooleanType : public Type
{
    BooleanType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const BooleanType* expected) const override;
};

struct StringType : public Type
{
    StringType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const StringType* expected) const override;
};

struct AudioSourceType : public Type
{
    AudioSourceType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const AudioSourceType* expected) const override;

    Token* getDefault(const SourceLocation location) const override;
};

struct EffectType : public Type
{
    EffectType();

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const EffectType* expected) const override;

    Token* getDefault(const SourceLocation location) const override;
};

struct ListType : public Type
{
    ListType(Type* subType);

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const ListType* expected) const override;

    Token* getDefault(const SourceLocation location) const override;

    Type* subType;
};

struct LambdaType : public Type
{
    LambdaType(const std::unordered_map<std::string, const Type*> inputTypes, const Type* returnType);

    bool checkType(const Type* actual) const override;

    bool checkSpecifiedType(const LambdaType* expected) const override;

    Token* getDefault(const SourceLocation location) const override;

private:
    std::string inputString(const std::unordered_map<std::string, const Type*>& inputTypes) const;

    const std::unordered_map<std::string, const Type*> inputTypes;

    const Type* returnType;

};

struct TypeResolver
{
    void resolveTypes(VariableRef* token);
    void resolveTypes(InputDef* token);
    void resolveTypes(InputRef* token);
    void resolveTypes(FunctionRef* token);
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
    void resolveArgumentTypes(ArgumentList* arguments, const std::string name, const Type* expectedType, Token* defaultValue = nullptr);

};

}
