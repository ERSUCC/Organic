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

    Token::Token(const SourceLocation location, const bool topLevel) :
        location(location), topLevel(topLevel) {}

    Type* Token::type(const BytecodeTransformer* visitor) const
    {
        return new Type(BasicType::Any);
    }

    void Token::accept(BytecodeTransformer* visitor) const {}

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
        BasicToken(location, str), value(value) {}

    Type* Value::type(const BytecodeTransformer* visitor) const
    {
        return new Type(BasicType::Number);
    }

    void Value::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    NamedConstant::NamedConstant(const SourceLocation location, const std::string constant) :
        BasicToken(location, constant) {}

    Type* NamedConstant::type(const BytecodeTransformer* visitor) const
    {
        if (str == "sequence-forwards" || str == "sequence-backwards" || str == "sequence-ping-pong" || str == "sequence-random")
        {
            return new Type(BasicType::SequenceOrder);
        }

        if (str == "random-step" || str == "random-linear")
        {
            return new Type(BasicType::RandomType);
        }

        if (str == "pi" || str == "e")
        {
            return new Type(BasicType::Number);
        }

        return new Type(BasicType::None);
    }

    void NamedConstant::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Variable::Variable(const SourceLocation location, const std::string variable) :
        BasicToken(location, variable) {}

    Type* Variable::type(const BytecodeTransformer* visitor) const
    {
        if (InputInfo* info = visitor->currentScope->getInput(str))
        {
            return info->token->type(visitor);
        }

        else if (VariableInfo* info = visitor->currentScope->getVariable(str))
        {
            return info->value->type(visitor);
        }

        Utils::parseError("Unrecognized variable name \"" + str + "\".", location);

        return new Type(BasicType::None);
    }

    void Variable::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Input::Input(const SourceLocation location, const std::string name) :
        BasicToken(location, name) {}

    Type* Input::type(const BytecodeTransformer* visitor) const
    {
        return assumedType;
    }

    String::String(const SourceLocation location, const std::string str) :
        BasicToken(location, str) {}

    Type* String::type(const BytecodeTransformer* visitor) const
    {
        return new Type(BasicType::String);
    }

    Argument::Argument(const SourceLocation location, const std::string name, const Token* value) :
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

    ArgumentList* ArgumentList::constructAlias(const std::vector<const Token*>& arguments, const std::string name)
    {
        std::vector<const Argument*> named;

        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            named.push_back(new Argument(arguments[i]->location, std::to_string(i), arguments[i]));
        }

        return new ArgumentList(named, name);
    }

    void ArgumentList::get(const std::string name, BytecodeTransformer* visitor, Type* expectedType)
    {
        count++;

        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                visitor->expectedType = expectedType;

                arguments[i]->value->accept(visitor);

                const Type* argumentType = arguments[i]->value->type(visitor);

                if (!argumentType->checkType(expectedType))
                {
                    Utils::parseError("Expected \"" + expectedType->name() + "\", received \"" + argumentType->name() + "\".", arguments[i]->value->location);
                }

                arguments.erase(arguments.begin() + i);

                return;
            }
        }

        visitor->currentScope->block->addInstruction(new StackPushDefault());
    }

    bool ArgumentList::has(const std::string name) const
    {
        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                return true;
            }
        }

        return false;
    }

    void ArgumentList::confirmEmpty() const
    {
        if (!arguments.empty())
        {
            Utils::parseError("Invalid input name \"" + arguments[0]->name + "\" for function \"" + name + "\".", arguments[0]->location);
        }
    }

    List::List(const SourceLocation location, const std::vector<const Token*> values) :
        Token(location), values(values) {}

    Type* List::type(const BytecodeTransformer* visitor) const
    {
        return new Type(BasicType::List, values[0]->type(visitor));
    }

    void List::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    ParenthesizedExpression::ParenthesizedExpression(const SourceLocation location, const Token* value) :
        Token(location), value(value) {}

    Type* ParenthesizedExpression::type(const BytecodeTransformer* visitor) const
    {
        return value->type(visitor);
    }

    void ParenthesizedExpression::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Assign::Assign(const SourceLocation location, const std::string variable, const Token* value) :
        Token(location, true), variable(variable), value(value) {}

    void Assign::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Call::Call(const SourceLocation location, const std::string name, ArgumentList* arguments, const bool topLevel) :
        Token(location, topLevel), name(name), arguments(arguments) {}

    Type* Call::type(const BytecodeTransformer* visitor) const
    {
        if (name == "time" ||
            name == "hold" ||
            name == "lfo" ||
            name == "sweep" ||
            name == "sequence" ||
            name == "repeat" ||
            name == "random" ||
            name == "limit" ||
            name == "trigger" ||
            name == "if")
        {
            return new Type(BasicType::Number);
        }

        if (name == "sine" ||
            name == "square" ||
            name == "triangle" ||
            name == "saw" ||
            name == "noise" ||
            name == "blend")
        {
            return new Type(BasicType::AudioSource);
        }

        if (name == "delay" ||
            name == "lowpass")
        {
            return new Type(BasicType::Effect);
        }

        if (FunctionInfo* info = visitor->currentScope->getFunction(name))
        {
            return info->token->type(visitor);
        }

        return new Type(BasicType::None);
    }

    void Call::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    CallAlias::CallAlias(const SourceLocation location, const std::string name, const std::vector<const Token*>& arguments) :
        Call(location, name, ArgumentList::constructAlias(arguments, name), false) {}

    Type* CallAlias::type(const BytecodeTransformer* visitor) const
    {
        if (name == "add" ||
            name == "subtract" ||
            name == "multiply" ||
            name == "divide" ||
            name == "power")
        {
            return new Type(BasicType::Number);
        }

        else if (name == "equal" ||
                 name == "less" ||
                 name == "greater" ||
                 name == "lessequal" ||
                 name == "greaterequal")
        {
            return new Type(BasicType::Boolean);
        }

        return new Type(BasicType::None);
    }

    void CallAlias::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Define::Define(const SourceLocation location, const std::string name, const std::vector<Input*> inputs, const std::vector<const Token*> instructions) :
        Token(location, true), name(name), inputs(inputs), instructions(instructions) {}

    Type* Define::type(const BytecodeTransformer* visitor) const
    {
        if (instructions.empty())
        {
            return new Type(BasicType::None);
        }

        return instructions.back()->type(visitor);
    }

    void Define::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Include::Include(const SourceLocation location, const Program* program) :
        Token(location, true), program(program) {}

    void Include::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
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
        block = new BytecodeBlock(inputs.size());

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
            if (!variablesUsed.count(pair.first))
            {
                Utils::parseWarning("Unused variable \"" + pair.first + "\".", pair.second->value->location);
            }
        }

        for (const std::pair<std::string, FunctionInfo*>& pair : functions)
        {
            if (!functionsUsed.count(pair.first))
            {
                Utils::parseWarning("Warning: Unused function \"" + pair.first + "\".", pair.second->token->location);
            }
        }

        for (const std::pair<std::string, InputInfo*>& pair : inputs)
        {
            if (!inputsUsed.count(pair.first))
            {
                Utils::parseWarning("Warning: Unused input \"" + pair.first + "\".", pair.second->token->location);
            }
        }
    }

    Program::Program(const SourceLocation location, const std::vector<const Token*> instructions) :
        Token(location, true), instructions(instructions) {}

    BytecodeTransformer::BytecodeTransformer(std::ofstream& outputStream) :
        outputStream(outputStream)
    {
        utils = Utils::get();

        currentScope = new Scope(this);
    }

    void BytecodeTransformer::visit(const Value* token)
    {
        currentScope->block->addInstruction(new StackPushDouble(token->value));
    }

    void BytecodeTransformer::visit(const NamedConstant* token)
    {
        if (token->str == "sequence-forwards")
        {
            currentScope->block->addInstruction(new StackPushByte(Sequence::OrderEnum::Forwards));
        }

        else if (token->str == "sequence-backwards")
        {
            currentScope->block->addInstruction(new StackPushByte(Sequence::OrderEnum::Backwards));
        }

        else if (token->str == "sequence-ping-pong")
        {
            currentScope->block->addInstruction(new StackPushByte(Sequence::OrderEnum::PingPong));
        }

        else if (token->str == "sequence-random")
        {
            currentScope->block->addInstruction(new StackPushByte(Sequence::OrderEnum::Random));
        }

        else if (token->str == "random-step")
        {
            currentScope->block->addInstruction(new StackPushByte(Random::TypeEnum::Step));
        }

        else if (token->str == "random-linear")
        {
            currentScope->block->addInstruction(new StackPushByte(Random::TypeEnum::Linear));
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

    void BytecodeTransformer::visit(const Variable* token)
    {
        if (const InputInfo* info = currentScope->getInput(token->str))
        {
            if (!info->token->type(this))
            {
                info->token->assumedType = expectedType;
            }

            currentScope->block->addInstruction(new GetVariable(info->id));
        }

        else if (const VariableInfo* info = currentScope->getVariable(token->str))
        {
            currentScope->block->addInstruction(new GetVariable(info->id));
        }

        else
        {
            Utils::parseError("Unrecognized variable name \"" + token->str + "\".", token->location);
        }
    }

    void BytecodeTransformer::visit(const List* token)
    {
        Type* innerType = expectedType->subType;

        for (int i = token->values.size() - 1; i >= 0; i--)
        {
            expectedType = innerType;

            token->values[i]->accept(this);
        }

        for (unsigned int i = 1; i < token->values.size(); i++)
        {
            if (!token->values[i]->type(this)->checkType(token->values[0]->type(this)))
            {
                Utils::parseError("All elements in a list must have the same type.", token->values[i]->location);
            }
        }

        currentScope->block->addInstruction(new StackPushInt(token->values.size()));
        currentScope->block->addInstruction(new CallNative("list", 1));
    }

    void BytecodeTransformer::visit(const ParenthesizedExpression* token)
    {
        token->value->accept(this);
    }

    void BytecodeTransformer::visit(const Assign* token)
    {
        token->value->accept(this);

        if (token->value->type(this)->checkType(new Type(BasicType::None)))
        {
            Utils::parseError("Functions that return nothing cannot be assigned to a variable.", token->value->location);
        }

        if (const InputInfo* info = currentScope->getInput(token->variable))
        {
            Utils::parseError("Function inputs cannot be redefined.", token->location);
        }

        else if (VariableInfo* info = currentScope->getVariable(token->variable))
        {
            Utils::parseError("Variables cannot be redefined.", token->location);
        }

        else
        {
            currentScope->block->addInstruction(new SetVariable(currentScope->addVariable(token->variable, token->value)->id));
        }
    }

    void BytecodeTransformer::visit(const Call* token)
    {
        if (const FunctionInfo* info = currentScope->getFunction(token->name))
        {
            for (const Input* input : info->scope->inputList)
            {
                if (Type* type = input->type(this))
                {
                    token->arguments->get(input->str, this, type);
                }

                else
                {
                    token->arguments->get(input->str, this);
                }

                currentScope->block->addInstruction(new SetVariable(info->scope->getInput(input->str)->id));
            }

            token->arguments->confirmEmpty();

            currentScope->block->addInstruction(new CallUser(info->scope->block));

            return;
        }

        bool audioSource = false;

        if (token->name == "time") {}

        else if (token->name == "sine" ||
                 token->name == "square" ||
                 token->name == "saw" ||
                 token->name == "triangle")
        {
            audioSource = true;

            token->arguments->get("frequency", this, new Type(BasicType::Number));
            token->arguments->get("effects", this, new Type(BasicType::List, new Type(BasicType::Effect)));
            token->arguments->get("pan", this, new Type(BasicType::Number));
            token->arguments->get("volume", this, new Type(BasicType::Number));
        }

        else if (token->name == "noise")
        {
            audioSource = true;

            token->arguments->get("effects", this, new Type(BasicType::List, new Type(BasicType::Effect)));
            token->arguments->get("pan", this, new Type(BasicType::Number));
            token->arguments->get("volume", this, new Type(BasicType::Number));
        }

        else if (token->name == "blend")
        {
            audioSource = true;

            token->arguments->get("position", this, new Type(BasicType::Number));
            token->arguments->get("sources", this, new Type(BasicType::List, new Type(BasicType::AudioSource)));
        }

        else if (token->name == "hold")
        {
            token->arguments->get("length", this, new Type(BasicType::Number));
            token->arguments->get("value", this, new Type(BasicType::Number));
        }

        else if (token->name == "lfo")
        {
            token->arguments->get("length", this, new Type(BasicType::Number));
            token->arguments->get("to", this, new Type(BasicType::Number));
            token->arguments->get("from", this, new Type(BasicType::Number));
        }

        else if (token->name == "sweep")
        {
            token->arguments->get("length", this, new Type(BasicType::Number));
            token->arguments->get("to", this, new Type(BasicType::Number));
            token->arguments->get("from", this, new Type(BasicType::Number));
        }

        else if (token->name == "sequence")
        {
            token->arguments->get("order", this, new Type(BasicType::SequenceOrder));
            token->arguments->get("values", this, new Type(BasicType::List, new Type(BasicType::Number)));
        }

        else if (token->name == "repeat")
        {
            token->arguments->get("repeats", this, new Type(BasicType::Number));
            token->arguments->get("value", this, new Type(BasicType::Number));
        }

        else if (token->name == "random")
        {
            token->arguments->get("type", this, new Type(BasicType::RandomType));
            token->arguments->get("length", this, new Type(BasicType::Number));
            token->arguments->get("to", this, new Type(BasicType::Number));
            token->arguments->get("from", this, new Type(BasicType::Number));
        }

        else if (token->name == "limit")
        {
            token->arguments->get("max", this, new Type(BasicType::Number));
            token->arguments->get("min", this, new Type(BasicType::Number));
            token->arguments->get("value", this, new Type(BasicType::Number));
        }

        else if (token->name == "trigger")
        {
            token->arguments->get("value", this, new Type(BasicType::Number));
            token->arguments->get("condition", this, new Type(BasicType::Boolean));
        }

        else if (token->name == "if")
        {
            token->arguments->get("false", this, new Type(BasicType::Number));
            token->arguments->get("true", this, new Type(BasicType::Number));
            token->arguments->get("condition", this, new Type(BasicType::Boolean));
        }

        else if (token->name == "delay")
        {
            token->arguments->get("feedback", this, new Type(BasicType::Number));
            token->arguments->get("delay", this, new Type(BasicType::Number));
            token->arguments->get("mix", this, new Type(BasicType::Number));
        }

        else if (token->name == "perform")
        {
            if (!token->arguments->has("function"))
            {
                return Utils::parseWarning("This function call does nothing and will be ignored.", token->location);
            }

            token->arguments->get("interval", this, new Type(BasicType::Number));
            token->arguments->get("repeats", this, new Type(BasicType::Number));
            token->arguments->get("delay", this, new Type(BasicType::Number));

            currentScope = new Scope(this, currentScope);

            token->arguments->get("function", this);

            BytecodeBlock* block = currentScope->block;

            resolver->addBlock(block);

            currentScope = currentScope->parent;

            currentScope->block->addInstruction(new StackPushAddress(block));
        }

        else if (currentScope->checkRecursive(token->name))
        {
            return Utils::parseError("Calling a function in its own definition is not allowed.", token->location);
        }

        else
        {
            return Utils::parseError("Unknown function name \"" + token->name + "\".", token->location);
        }

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(token->name, token->arguments->count));

        if (audioSource && token->topLevel)
        {
            currentScope->block->addInstruction(new CallNative("play", 1));
        }
    }

    void BytecodeTransformer::visit(const CallAlias* token)
    {
        if (token->name == "add")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "subtract")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "multiply")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "divide")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "power")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "equal")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "less")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "greater")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "lessequal")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        else if (token->name == "greaterequal")
        {
            token->arguments->get("1", this, new Type(BasicType::Number));
            token->arguments->get("0", this, new Type(BasicType::Number));
        }

        currentScope->block->addInstruction(new CallNative(token->name, token->arguments->count));
    }

    void BytecodeTransformer::visit(const Define* token)
    {
        if (token->name == "sine" ||
            token->name == "square" ||
            token->name == "saw" ||
            token->name == "triangle" ||
            token->name == "noise" ||
            token->name == "blend" ||
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
            return Utils::parseError("A function already exists with the name \"" + token->name + "\".", token->location);
        }

        if (currentScope->checkRecursive(token->name))
        {
            return Utils::parseError("Redefining a function in its own definition is not allowed.", token->location);
        }

        currentScope = new Scope(this, currentScope, token->name, token->inputs);

        for (const Token* instruction : token->instructions)
        {
            instruction->accept(this);
        }

        currentScope->checkUses();

        BytecodeBlock* block = currentScope->block;

        resolver->addBlock(block);

        currentScope->parent->addFunction(token->name, currentScope, token);

        currentScope = currentScope->parent;
    }

    void BytecodeTransformer::visit(const Include* token)
    {
        const std::string sourcePath = token->program->location.path;

        if (includedPaths.count(sourcePath))
        {
            Utils::error("Source file \"" + sourcePath + "\" has already been included.");
        }

        includedPaths.insert(sourcePath);

        for (const Token* instruction : token->program->instructions)
        {
            instruction->accept(this);
        }
    }

    void BytecodeTransformer::visit(const Program* token)
    {
        includedPaths.insert(token->location.path);

        resolver->addBlock(currentScope->block);

        for (const Token* instruction : token->instructions)
        {
            instruction->accept(this);
        }

        currentScope->checkUses();

        resolver->output(outputStream, nextVariable);
    }

    unsigned char BytecodeTransformer::newVariableId()
    {
        return nextVariable++;
    }
}
