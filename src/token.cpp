#include "../include/token.h"

namespace Parser
{
    Type::Type(const BasicType primaryType, Type* subType) :
        primaryType(primaryType), subType(subType) {}

    std::string Type::name() const
    {
        switch (primaryType)
        {
            case BasicType::None:
                return "nothing";

            case BasicType::Any:
                return "anything";

            case BasicType::SequenceOrder:
                return "sequence order constant";

            case BasicType::RandomType:
                return "random type constant";

            case BasicType::Number:
                return "number";

            case BasicType::Boolean:
                return "boolean";

            case BasicType::String:
                return "string";

            case BasicType::List:
                return "list of " + subType->name();

            case BasicType::AudioSource:
                return "audio source";

            case BasicType::Effect:
                return "effect";
        }

        return "";
    }

    bool Type::checkType(const Type* expected) const
    {
        if (!expected)
        {
            return false;
        }

        switch (expected->primaryType)
        {
            case BasicType::None:
                if (primaryType != BasicType::None)
                {
                    return false;
                }

                break;

            case BasicType::Any:
                break;

            case BasicType::SequenceOrder:
                if (primaryType != BasicType::SequenceOrder)
                {
                    return false;
                }

                break;

            case BasicType::RandomType:
                if (primaryType != BasicType::RandomType)
                {
                    return false;
                }

                break;

            case BasicType::Number:
                if (primaryType != BasicType::Number)
                {
                    return false;
                }

                break;

            case BasicType::Boolean:
                if (primaryType != BasicType::Boolean)
                {
                    return false;
                }

                break;

            case BasicType::String:
                if (primaryType != BasicType::String)
                {
                    return false;
                }

                break;

            case BasicType::List:
                if (primaryType != BasicType::List)
                {
                    return false;
                }

                break;

            case BasicType::AudioSource:
                if (primaryType != BasicType::AudioSource)
                {
                    return false;
                }

                break;

            case BasicType::Effect:
                if (primaryType != BasicType::Effect)
                {
                    return false;
                }

                break;
        }

        if (subType)
        {
            return subType->checkType(expected->subType);
        }

        return !expected->subType;
    }

    Token::Token(const SourceLocation location) :
        location(location) {}

    void Token::resolveTypes(BytecodeTransformer* visitor) {}
    void Token::transform(BytecodeTransformer* visitor) const {}

    BasicToken::BasicToken(const SourceLocation location, const std::string str) :
        Token(location), str(str) {}

    OpenParenthesis::OpenParenthesis(const SourceLocation location) :
        BasicToken(location, "(") {}

    CloseParenthesis::CloseParenthesis(const SourceLocation location) :
        BasicToken(location, ")") {}

    OpenSquareBracket::OpenSquareBracket(const SourceLocation location) :
        BasicToken(location, "[") {}

    CloseSquareBracket::CloseSquareBracket(const SourceLocation location) :
        BasicToken(location, "]") {}

    OpenCurlyBracket::OpenCurlyBracket(const SourceLocation location) :
        BasicToken(location, "{") {}

    CloseCurlyBracket::CloseCurlyBracket(const SourceLocation location) :
        BasicToken(location, "}") {}

    Colon::Colon(const SourceLocation location) :
        BasicToken(location, ":") {}

    Comma::Comma(const SourceLocation location) :
        BasicToken(location, ",") {}

    Equals::Equals(const SourceLocation location) :
        BasicToken(location, "=") {}

    Operator::Operator(const SourceLocation location, const std::string str) :
        BasicToken(location, str) {}

    Add::Add(const SourceLocation location) :
        Operator(location, "+") {}

    Subtract::Subtract(const SourceLocation location) :
        Operator(location, "-") {}

    Multiply::Multiply(const SourceLocation location) :
        Operator(location, "*") {}

    Divide::Divide(const SourceLocation location) :
        Operator(location, "/") {}

    Power::Power(const SourceLocation location) :
        Operator(location, "^") {}

    DoubleEquals::DoubleEquals(const SourceLocation location) :
        Operator(location, "==") {}

    Less::Less(const SourceLocation location) :
        Operator(location, "<") {}

    Greater::Greater(const SourceLocation location) :
        Operator(location, ">") {}

    LessEqual::LessEqual(const SourceLocation location) :
        Operator(location, "<=") {}

    GreaterEqual::GreaterEqual(const SourceLocation location) :
        Operator(location, ">=") {}

    Identifier::Identifier(const SourceLocation location, const std::string str) :
        BasicToken(location, str) {}

    Value::Value(const SourceLocation location, const std::string str, const double value) :
        BasicToken(location, str), value(value)
    {
        type = new Type(BasicType::Number);
    }

    void Value::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    NamedConstant::NamedConstant(const SourceLocation location, const std::string constant) :
        BasicToken(location, constant)
    {
        if (constant == "sequence-forwards" || constant == "sequence-backwards" || constant == "sequence-ping-pong" || constant == "sequence-random")
        {
            type = new Type(BasicType::SequenceOrder);
        }

        else if (constant == "random-step" || constant == "random-linear")
        {
            type = new Type(BasicType::RandomType);
        }

        else if (constant == "pi" || constant == "e")
        {
            type = new Type(BasicType::Number);
        }
    }

    void NamedConstant::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Variable::Variable(const SourceLocation location, const std::string variable) :
        BasicToken(location, variable) {}

    void Variable::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Variable::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Input::Input(const SourceLocation location, const std::string name) :
        BasicToken(location, name) {}

    String::String(const SourceLocation location, const std::string str) :
        BasicToken(location, str)
    {
        type = new Type(BasicType::String);
    }

    Ignore::Ignore(const SourceLocation location) :
        Token(location) {}

    Argument::Argument(const SourceLocation location, const std::string name, Token* value) :
        Token(location), name(name), value(value) {}

    ArgumentList::ArgumentList(const std::vector<const Argument*> arguments, const std::string name) :
        arguments(arguments), name(name)
    {
        std::unordered_set<std::string> defined;

        for (const Argument* argument : arguments)
        {
            if (defined.count(argument->name))
            {
                Utils::parseError("Input \"" + argument->name + "\" specified more than once in function \"" + name + "\".", argument->location);
            }

            defined.insert(argument->name);
        }
    }

    ArgumentList* ArgumentList::constructAlias(const std::vector<Token*>& arguments, const std::string name)
    {
        std::vector<const Argument*> named;

        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            named.push_back(new Argument(arguments[i]->location, std::to_string(i), arguments[i]));
        }

        return new ArgumentList(named, name);
    }

    void ArgumentList::resolveTypes(const std::string name, BytecodeTransformer* visitor, Type* expectedType)
    {
        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                visitor->expectedType = expectedType;

                arguments[i]->value->resolveTypes(visitor);

                const Type* argumentType = arguments[i]->value->type;

                if (!argumentType->checkType(expectedType))
                {
                    Utils::parseError("Expected \"" + expectedType->name() + "\", received \"" + argumentType->name() + "\".", arguments[i]->value->location);
                }

                return;
            }
        }
    }

    void ArgumentList::transform(const std::string name, BytecodeTransformer* visitor)
    {
        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                arguments[i]->value->transform(visitor);

                arguments.erase(arguments.begin() + i);

                return;
            }
        }

        visitor->currentScope->block->addInstruction(new StackPushDefault());
    }

    const Token* ArgumentList::get(const std::string name, BytecodeTransformer* visitor)
    {
        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                const Token* value = arguments[i]->value;

                arguments.erase(arguments.begin() + i);

                return value;
            }
        }

        return nullptr;
    }

    void ArgumentList::confirmEmpty() const
    {
        if (!arguments.empty())
        {
            Utils::parseError("Invalid input name \"" + arguments[0]->name + "\" for function \"" + name + "\".", arguments[0]->location);
        }
    }

    List::List(const SourceLocation location, const std::vector<Token*> values) :
        Token(location), values(values) {}

    void List::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void List::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    ParenthesizedExpression::ParenthesizedExpression(const SourceLocation location, Token* value) :
        Token(location), value(value) {}

    void ParenthesizedExpression::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void ParenthesizedExpression::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Assign::Assign(const SourceLocation location, const std::string variable, Token* value) :
        Token(location), variable(variable), value(value) {}

    void Assign::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Assign::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Call::Call(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Token(location), arguments(arguments), topLevel(topLevel) {}

    Time::Time(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        arguments->confirmEmpty();

        type = new Type(BasicType::Number);
    }

    void Time::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Hold::Hold(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Hold::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Hold::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    LFO::LFO(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void LFO::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void LFO::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sweep::Sweep(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Sweep::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sweep::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sequence::Sequence(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Sequence::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sequence::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Repeat::Repeat(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Repeat::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Repeat::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Random::Random(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Random::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Random::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Limit::Limit(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Limit::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Limit::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Trigger::Trigger(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void Trigger::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Trigger::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    If::If(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Number);
    }

    void If::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void If::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sine::Sine(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Sine::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sine::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Square::Square(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Square::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Square::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Triangle::Triangle(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Triangle::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Triangle::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Saw::Saw(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Saw::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Saw::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Noise::Noise(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Noise::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Noise::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Sample::Sample(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::AudioSource);
    }

    void Sample::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Sample::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Delay::Delay(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::Effect);
    }

    void Delay::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Delay::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Perform::Perform(const SourceLocation location, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel)
    {
        type = new Type(BasicType::None);
    }

    void Perform::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Perform::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    CallUser::CallUser(const SourceLocation location, const std::string name, ArgumentList* arguments, const bool topLevel) :
        Call(location, arguments, topLevel), name(name) {}

    void CallUser::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void CallUser::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    CallAlias::CallAlias(const SourceLocation location, Token* a, Token* b) :
        Token(location), a(a), b(b) {}

    void CallAlias::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    AddAlias::AddAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Number);
    }

    void AddAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    SubtractAlias::SubtractAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Number);
    }

    void SubtractAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    MultiplyAlias::MultiplyAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Number);
    }

    void MultiplyAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    DivideAlias::DivideAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Number);
    }

    void DivideAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    PowerAlias::PowerAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Number);
    }

    void PowerAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    EqualAlias::EqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Boolean);
    }

    void EqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    LessAlias::LessAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Boolean);
    }

    void LessAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    GreaterAlias::GreaterAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Boolean);
    }

    void GreaterAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    LessEqualAlias::LessEqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Boolean);
    }

    void LessEqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    GreaterEqualAlias::GreaterEqualAlias(const SourceLocation location, Token* a, Token* b) :
        CallAlias(location, a, b)
    {
        type = new Type(BasicType::Boolean);
    }

    void GreaterEqualAlias::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    Define::Define(const SourceLocation location, const std::string name, const std::vector<Input*> inputs, const std::vector<Token*> instructions) :
        Token(location), name(name), inputs(inputs), instructions(instructions) {}

    void Define::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Define::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    VariableInfo::VariableInfo(const unsigned char id, const Token* value) :
        id(id), value(value) {}

    FunctionInfo::FunctionInfo(Scope* scope, const Define* token) :
        scope(scope), token(token) {}

    InputInfo::InputInfo(const unsigned char id, Input* token) :
        id(id), token(token) {}

    Scope::Scope(BytecodeTransformer* visitor, Scope* parent, const std::string currentFunction, const std::vector<Input*> inputs) :
        visitor(visitor), parent(parent), currentFunction(currentFunction), inputList(inputs)
    {
        block = new InstructionBlock(inputs.size());

        for (Input* input : inputs)
        {
            this->inputs[input->str] = new InputInfo(visitor->newVariableId(), input);
        }
    }

    VariableInfo* Scope::getVariable(const std::string name)
    {
        if (variables.count(name))
        {
            variablesUsed.insert(name);

            return variables[name];
        }

        if (parent)
        {
            if (VariableInfo* info = parent->getVariable(name))
            {
                return info;
            }
        }

        return nullptr;
    }

    VariableInfo* Scope::addVariable(const std::string name, const Token* value)
    {
        VariableInfo* info = new VariableInfo(visitor->newVariableId(), value);

        variables[name] = info;

        return info;
    }

    FunctionInfo* Scope::getFunction(const std::string name)
    {
        if (functions.count(name))
        {
            functionsUsed.insert(name);

            return functions[name];
        }

        if (parent)
        {
            if (FunctionInfo* info = parent->getFunction(name))
            {
                return info;
            }
        }

        return nullptr;
    }

    FunctionInfo* Scope::addFunction(const std::string name, Scope* scope, const Define* token)
    {
        FunctionInfo* info = new FunctionInfo(scope, token);

        functions[name] = info;

        return info;
    }

    InputInfo* Scope::getInput(const std::string name)
    {
        if (inputs.count(name))
        {
            inputsUsed.insert(name);

            return inputs[name];
        }

        if (parent)
        {
            if (InputInfo* info = parent->getInput(name))
            {
                return info;
            }
        }

        return nullptr;
    }

    bool Scope::checkRecursive(const std::string function) const
    {
        return currentFunction == function || (parent && parent->checkRecursive(function));
    }

    void Scope::checkUses() const
    {
        for (const std::pair<std::string, VariableInfo*>& pair : variables)
        {
            if (!variablesUsed.count(pair.first) && pair.second->value->location.path == visitor->sourcePath)
            {
                Utils::parseWarning("Unused variable \"" + pair.first + "\".", pair.second->value->location);
            }
        }

        for (const std::pair<std::string, FunctionInfo*>& pair : functions)
        {
            if (!functionsUsed.count(pair.first) && pair.second->token->location.path == visitor->sourcePath)
            {
                Utils::parseWarning("Warning: Unused function \"" + pair.first + "\".", pair.second->token->location);
            }
        }

        for (const std::pair<std::string, InputInfo*>& pair : inputs)
        {
            if (!inputsUsed.count(pair.first) && pair.second->token->location.path == visitor->sourcePath)
            {
                Utils::parseWarning("Warning: Unused input \"" + pair.first + "\".", pair.second->token->location);
            }
        }
    }

    Program::Program(const SourceLocation location, const std::vector<Token*> instructions) :
        Token(location), instructions(instructions) {}

    Include::Include(const SourceLocation location, Program* program) :
        Token(location), program(program) {}

    void Include::resolveTypes(BytecodeTransformer* visitor)
    {
        visitor->resolveTypes(this);
    }

    void Include::transform(BytecodeTransformer* visitor) const
    {
        visitor->transform(this);
    }

    BytecodeTransformer::BytecodeTransformer(const Path* sourcePath, std::ofstream& outputStream) :
        sourcePath(sourcePath), outputStream(outputStream)
    {
        utils = Utils::get();

        currentScope = new Scope(this);
    }

    void BytecodeTransformer::createBytecode(Program* program)
    {
        for (Token* instruction : program->instructions)
        {
            instruction->resolveTypes(this);
        }

        currentScope->checkUses();

        resolver->addInstructionBlock(currentScope->block);

        for (const Token* instruction : program->instructions)
        {
            instruction->transform(this);
        }

        resolver->output(outputStream, nextVariable);
    }

    void BytecodeTransformer::resolveTypes(Variable* token)
    {
        if (const InputInfo* info = currentScope->getInput(token->str))
        {
            if (!token->type)
            {
                token->type = expectedType;
                info->token->type = expectedType;
            }
        }

        else if (const VariableInfo* info = currentScope->getVariable(token->str))
        {
            token->type = info->value->type;
        }

        else
        {
            Utils::parseError("Unrecognized variable name \"" + token->str + "\".", token->location);
        }
    }

    void BytecodeTransformer::resolveTypes(List* token)
    {
        Type* innerType = expectedType->subType;

        for (Token* value : token->values)
        {
            expectedType = innerType;

            value->resolveTypes(this);

            if (!value->type->checkType(token->values[0]->type))
            {
                Utils::parseError("All elements in a list must have the same type.", value->location);
            }
        }

        token->type = new Type(BasicType::List, token->values[0]->type);
    }

    void BytecodeTransformer::resolveTypes(ParenthesizedExpression* token)
    {
        token->value->resolveTypes(this);

        token->type = token->value->type;
    }

    void BytecodeTransformer::resolveTypes(Assign* token)
    {
        token->value->resolveTypes(this);

        if (token->value->type->checkType(new Type(BasicType::None)))
        {
            Utils::parseError("Functions that return nothing cannot be assigned to a variable.", token->value->location);
        }

        if (const InputInfo* info = currentScope->getInput(token->variable))
        {
            Utils::parseError("Function inputs cannot be redefined.", token->location);
        }

        if (const VariableInfo* info = currentScope->getVariable(token->variable))
        {
            Utils::parseError("Variables cannot be redefined.", token->location);
        }

        currentScope->addVariable(token->variable, token->value);
    }

    void BytecodeTransformer::resolveTypes(Hold* token)
    {
        token->arguments->resolveTypes("length", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("value", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(LFO* token)
    {
        token->arguments->resolveTypes("length", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("to", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("from", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Sweep* token)
    {
        token->arguments->resolveTypes("length", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("to", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("from", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Sequence* token)
    {
        token->arguments->resolveTypes("order", this, new Type(BasicType::SequenceOrder));
        token->arguments->resolveTypes("values", this, new Type(BasicType::List, new Type(BasicType::Number)));
    }

    void BytecodeTransformer::resolveTypes(Repeat* token)
    {
        token->arguments->resolveTypes("repeats", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("value", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Random* token)
    {
        token->arguments->resolveTypes("type", this, new Type(BasicType::RandomType));
        token->arguments->resolveTypes("length", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("to", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("from", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Limit* token)
    {
        token->arguments->resolveTypes("max", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("min", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("value", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Trigger* token)
    {
        token->arguments->resolveTypes("value", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("condition", this, new Type(BasicType::Boolean));
    }

    void BytecodeTransformer::resolveTypes(If* token)
    {
        token->arguments->resolveTypes("false", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("true", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("condition", this, new Type(BasicType::Boolean));
    }

    void BytecodeTransformer::resolveTypes(Sine* token)
    {
        token->arguments->resolveTypes("frequency", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("effects", this, new Type(BasicType::List, new Type(BasicType::Effect)));
        token->arguments->resolveTypes("pan", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("volume", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Square* token)
    {
        token->arguments->resolveTypes("frequency", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("effects", this, new Type(BasicType::List, new Type(BasicType::Effect)));
        token->arguments->resolveTypes("pan", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("volume", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Triangle* token)
    {
        token->arguments->resolveTypes("frequency", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("effects", this, new Type(BasicType::List, new Type(BasicType::Effect)));
        token->arguments->resolveTypes("pan", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("volume", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Saw* token)
    {
        token->arguments->resolveTypes("frequency", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("effects", this, new Type(BasicType::List, new Type(BasicType::Effect)));
        token->arguments->resolveTypes("pan", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("volume", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Noise* token)
    {
        token->arguments->resolveTypes("effects", this, new Type(BasicType::List, new Type(BasicType::Effect)));
        token->arguments->resolveTypes("pan", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("volume", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Sample* token)
    {
        token->arguments->resolveTypes("file", this, new Type(BasicType::String));
        token->arguments->resolveTypes("effects", this, new Type(BasicType::List, new Type(BasicType::Effect)));
        token->arguments->resolveTypes("pan", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("volume", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Delay* token)
    {
        token->arguments->resolveTypes("feedback", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("delay", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("mix", this, new Type(BasicType::Number));
    }

    void BytecodeTransformer::resolveTypes(Perform* token)
    {
        token->arguments->resolveTypes("interval", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("repeats", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("delay", this, new Type(BasicType::Number));
        token->arguments->resolveTypes("function", this, new Type(BasicType::Any));
    }

    void BytecodeTransformer::resolveTypes(CallUser* token)
    {
        if (const FunctionInfo* info = currentScope->getFunction(token->name))
        {
            for (const Input* input : info->scope->inputList)
            {
                if (Type* type = input->type)
                {
                    token->arguments->resolveTypes(input->str, this, type);
                }

                else
                {
                    token->arguments->resolveTypes(input->str, this, new Type(BasicType::Any));
                }
            }
        }

        else if (currentScope->checkRecursive(token->name))
        {
            Utils::parseError("Calling a function in its own definition is not allowed.", token->location);
        }

        else
        {
            Utils::parseError("Unknown function name \"" + token->name + "\".", token->location);
        }
    }

    void BytecodeTransformer::resolveTypes(CallAlias* token)
    {
        token->a->resolveTypes(this);
        token->b->resolveTypes(this);

        const Type* number = new Type(BasicType::Number);

        if (!token->a->type->checkType(number))
        {
            Utils::parseError("Expected \"" + number->name() + "\", received \"" + token->a->type->name() + "\".", token->a->location);
        }

        if (!token->b->type->checkType(number))
        {
            Utils::parseError("Expected \"" + number->name() + "\", received \"" + token->b->type->name() + "\".", token->b->location);
        }
    }

    void BytecodeTransformer::resolveTypes(Define* token)
    {
        if (token->name == "sine" ||
            token->name == "square" ||
            token->name == "saw" ||
            token->name == "triangle" ||
            token->name == "noise" ||
            token->name == "sample" ||
            token->name == "hold" ||
            token->name == "lfo" ||
            token->name == "sweep" ||
            token->name == "sequence" ||
            token->name == "repeat" ||
            token->name == "random" ||
            token->name == "limit" ||
            token->name == "trigger" ||
            token->name == "if" ||
            token->name == "delay" ||
            token->name == "lowpass" ||
            token->name == "play" ||
            token->name == "perform" ||
            currentScope->getFunction(token->name))
        {
            Utils::parseError("A function already exists with the name \"" + token->name + "\".", token->location);
        }

        if (currentScope->checkRecursive(token->name))
        {
            Utils::parseError("Redefining a function in its own definition is not allowed.", token->location);
        }

        currentScope = new Scope(this, currentScope, token->name, token->inputs);

        for (Token* instruction : token->instructions)
        {
            instruction->resolveTypes(this);
        }

        currentScope->checkUses();

        currentScope->parent->addFunction(token->name, currentScope, token);

        currentScope = currentScope->parent;

        if (token->instructions.empty())
        {
            // should this be a warning?

            token->type = new Type(BasicType::None);
        }

        else
        {
            token->type = token->instructions.back()->type;
        }
    }

    void BytecodeTransformer::resolveTypes(Include* token)
    {
        token->program->resolveTypes(this);
    }

    void BytecodeTransformer::transform(const Value* token)
    {
        currentScope->block->addInstruction(new StackPushDouble(token->value));
    }

    void BytecodeTransformer::transform(const NamedConstant* token)
    {
        if (token->str == "sequence-forwards")
        {
            currentScope->block->addInstruction(new StackPushByte(::Sequence::OrderEnum::Forwards));
        }

        else if (token->str == "sequence-backwards")
        {
            currentScope->block->addInstruction(new StackPushByte(::Sequence::OrderEnum::Backwards));
        }

        else if (token->str == "sequence-ping-pong")
        {
            currentScope->block->addInstruction(new StackPushByte(::Sequence::OrderEnum::PingPong));
        }

        else if (token->str == "sequence-random")
        {
            currentScope->block->addInstruction(new StackPushByte(::Sequence::OrderEnum::Random));
        }

        else if (token->str == "random-step")
        {
            currentScope->block->addInstruction(new StackPushByte(::Random::TypeEnum::Step));
        }

        else if (token->str == "random-linear")
        {
            currentScope->block->addInstruction(new StackPushByte(::Random::TypeEnum::Linear));
        }

        else if (token->str == "pi")
        {
            currentScope->block->addInstruction(new StackPushDouble(utils->pi));
        }

        else if (token->str == "e")
        {
            currentScope->block->addInstruction(new StackPushDouble(utils->e));
        }
    }

    void BytecodeTransformer::transform(const Variable* token)
    {
        if (const InputInfo* info = currentScope->getInput(token->str))
        {
            currentScope->block->addInstruction(new GetVariable(info->id));
        }

        else if (const VariableInfo* info = currentScope->getVariable(token->str))
        {
            currentScope->block->addInstruction(new GetVariable(info->id));
        }
    }

    void BytecodeTransformer::transform(const List* token)
    {
        for (int i = token->values.size() - 1; i >= 0; i--)
        {
            token->values[i]->transform(this);
        }

        currentScope->block->addInstruction(new StackPushInt(token->values.size()));
        currentScope->block->addInstruction(new CallNative(BytecodeConstants::LIST, 1));
    }

    void BytecodeTransformer::transform(const ParenthesizedExpression* token)
    {
        token->value->transform(this);
    }

    void BytecodeTransformer::transform(const Assign* token)
    {
        token->value->transform(this);

        currentScope->block->addInstruction(new SetVariable(currentScope->getVariable(token->variable)->id));
    }

    void BytecodeTransformer::transform(const Time* token)
    {
        currentScope->block->addInstruction(new CallNative(BytecodeConstants::TIME, 0));
    }

    void BytecodeTransformer::transform(const Hold* token)
    {
        token->arguments->transform("length", this);
        token->arguments->transform("value", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::HOLD, 2));
    }

    void BytecodeTransformer::transform(const LFO* token)
    {
        token->arguments->transform("length", this);
        token->arguments->transform("to", this);
        token->arguments->transform("from", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::LFO, 3));
    }

    void BytecodeTransformer::transform(const Sweep* token)
    {
        token->arguments->transform("length", this);
        token->arguments->transform("to", this);
        token->arguments->transform("from", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::SWEEP, 3));
    }

    void BytecodeTransformer::transform(const Sequence* token)
    {
        token->arguments->transform("order", this);
        token->arguments->transform("values", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::SEQUENCE, 2));
    }

    void BytecodeTransformer::transform(const Repeat* token)
    {
        token->arguments->transform("repeats", this);
        token->arguments->transform("value", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::REPEAT, 2));
    }

    void BytecodeTransformer::transform(const Random* token)
    {
        token->arguments->transform("type", this);
        token->arguments->transform("length", this);
        token->arguments->transform("to", this);
        token->arguments->transform("from", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::RANDOM, 4));
    }

    void BytecodeTransformer::transform(const Limit* token)
    {
        token->arguments->transform("max", this);
        token->arguments->transform("min", this);
        token->arguments->transform("value", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::LIMIT, 3));
    }

    void BytecodeTransformer::transform(const Trigger* token)
    {
        token->arguments->transform("value", this);
        token->arguments->transform("condition", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::TRIGGER, 2));
    }

    void BytecodeTransformer::transform(const If* token)
    {
        token->arguments->transform("false", this);
        token->arguments->transform("true", this);
        token->arguments->transform("condition", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::IF, 3));
    }

    void BytecodeTransformer::transform(const Sine* token)
    {
        token->arguments->transform("frequency", this);
        token->arguments->transform("effects", this);
        token->arguments->transform("pan", this);
        token->arguments->transform("volume", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::SINE, 4));

        if (token->topLevel)
        {
            currentScope->block->addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Square* token)
    {
        token->arguments->transform("frequency", this);
        token->arguments->transform("effects", this);
        token->arguments->transform("pan", this);
        token->arguments->transform("volume", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::SQUARE, 4));

        if (token->topLevel)
        {
            currentScope->block->addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Triangle* token)
    {
        token->arguments->transform("frequency", this);
        token->arguments->transform("effects", this);
        token->arguments->transform("pan", this);
        token->arguments->transform("volume", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::TRIANGLE, 4));

        if (token->topLevel)
        {
            currentScope->block->addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Saw* token)
    {
        token->arguments->transform("frequency", this);
        token->arguments->transform("effects", this);
        token->arguments->transform("pan", this);
        token->arguments->transform("volume", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::SAW, 4));

        if (token->topLevel)
        {
            currentScope->block->addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Noise* token)
    {
        token->arguments->transform("effects", this);
        token->arguments->transform("pan", this);
        token->arguments->transform("volume", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::NOISE, 3));

        if (token->topLevel)
        {
            currentScope->block->addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Sample* token)
    {
        const String* str = dynamic_cast<const String*>(token->arguments->get("file", this));

        const Path* path = Path::beside(str->str, sourcePath);

        if (!path->exists())
        {
            Utils::includeError("Audio file \"" + path->string() + "\" does not exist.", str->location);
        }

        if (!path->isFile())
        {
            Utils::includeError("\"" + path->string() + "\" is not a file.", str->location);
        }

        if (!resources.count(path))
        {
            resources[path] = resources.size();

            resolver->addResourceBlock(new ResourceBlock(path));
        }

        currentScope->block->addInstruction(new StackPushResource(resources[path]));

        token->arguments->transform("effects", this);
        token->arguments->transform("pan", this);
        token->arguments->transform("volume", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::SAMPLE, 4));

        if (token->topLevel)
        {
            currentScope->block->addInstruction(new CallNative(BytecodeConstants::PLAY, 1));
        }
    }

    void BytecodeTransformer::transform(const Delay* token)
    {
        token->arguments->transform("feedback", this);
        token->arguments->transform("delay", this);
        token->arguments->transform("mix", this);

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::DELAY, 3));
    }

    void BytecodeTransformer::transform(const Perform* token)
    {
        token->arguments->transform("interval", this);
        token->arguments->transform("repeats", this);
        token->arguments->transform("delay", this);

        currentScope = new Scope(this, currentScope);

        token->arguments->transform("function", this);

        InstructionBlock* block = currentScope->block;

        resolver->addInstructionBlock(block);

        currentScope = currentScope->parent;

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new StackPushAddress(block));
        currentScope->block->addInstruction(new CallNative(BytecodeConstants::PERFORM, 4));
    }

    void BytecodeTransformer::transform(const CallUser* token)
    {
        const FunctionInfo* info = currentScope->getFunction(token->name);

        for (const Input* input : info->scope->inputList)
        {
            token->arguments->transform(input->str, this);

            currentScope->block->addInstruction(new SetVariable(info->scope->getInput(input->str)->id));
        }

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new ::CallUser(info->scope->block));
    }

    void BytecodeTransformer::transform(const AddAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::ADD, 2));
    }

    void BytecodeTransformer::transform(const SubtractAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::SUBTRACT, 2));
    }

    void BytecodeTransformer::transform(const MultiplyAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::MULTIPLY, 2));
    }

    void BytecodeTransformer::transform(const DivideAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::DIVIDE, 2));
    }

    void BytecodeTransformer::transform(const PowerAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::POWER, 2));
    }

    void BytecodeTransformer::transform(const EqualAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::EQUAL, 2));
    }

    void BytecodeTransformer::transform(const LessAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::LESS, 2));
    }

    void BytecodeTransformer::transform(const GreaterAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::GREATER, 2));
    }

    void BytecodeTransformer::transform(const LessEqualAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::LESSEQUAL, 2));
    }

    void BytecodeTransformer::transform(const GreaterEqualAlias* token)
    {
        token->b->transform(this);
        token->a->transform(this);

        currentScope->block->addInstruction(new CallNative(BytecodeConstants::GREATEREQUAL, 2));
    }

    void BytecodeTransformer::transform(const Define* token)
    {
        const FunctionInfo* info = currentScope->getFunction(token->name);

        currentScope = info->scope;

        for (const Token* instruction : token->instructions)
        {
            instruction->transform(this);
        }

        resolver->addInstructionBlock(currentScope->block);

        currentScope = currentScope->parent;
    }

    void BytecodeTransformer::transform(const Include* token)
    {
        for (const Token* instruction : token->program->instructions)
        {
            instruction->transform(this);
        }
    }

    unsigned char BytecodeTransformer::newVariableId()
    {
        return nextVariable++;
    }
}
