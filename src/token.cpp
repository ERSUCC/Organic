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
            
            case BasicType::List:
                return "list of " + subType->name();
            
            case BasicType::AudioSource:
                return "audio source";
            
            case BasicType::Effect:
                return "effect";
        }

        return "";
    }
    
    bool Type::checkType(const Type* other) const
    {
        if (!other)
        {
            return false;
        }

        switch (primaryType)
        {
            case BasicType::None:
                if (other->primaryType != BasicType::None)
                {
                    return false;
                }

                break;
            
            case BasicType::Any:
                break;
            
            case BasicType::SequenceOrder:
                if (other->primaryType != BasicType::SequenceOrder)
                {
                    return false;
                }

                break;
            
            case BasicType::RandomType:
                if (other->primaryType != BasicType::RandomType)
                {
                    return false;
                }

                break;
            
            case BasicType::Number:
                if (other->primaryType != BasicType::Number)
                {
                    return false;
                }

                break;
            
            case BasicType::Boolean:
                if (other->primaryType != BasicType::Boolean)
                {
                    return false;
                }

                break;

            case BasicType::List:
                if (other->primaryType != BasicType::List)
                {
                    return false;
                }

                break;

            case BasicType::AudioSource:
                if (other->primaryType != BasicType::AudioSource)
                {
                    return false;
                }

                break;

            case BasicType::Effect:
                if (other->primaryType != BasicType::Effect)
                {
                    return false;
                }

                break;
        }

        if (subType)
        {
            return subType->checkType(other->subType);
        }

        return !other->subType;
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

    String::String(const SourceLocation location, const std::string str) :
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
        if (VariableInfo* info = visitor->currentScope->getVariable(str))
        {
            return info->value->type(visitor);
        }

        else if (InputInfo* info = visitor->currentScope->getInput(str))
        {
            return info->type;
        }

        Utils::parseError("Unrecognized variable name \"" + str + "\".", location);

        return new Type(BasicType::None);
    }

    void Variable::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
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

                if (!expectedType->checkType(argumentType))
                {
                    Utils::parseError("Expected \"" + expectedType->name() + "\", received \"" + argumentType->name() + "\".", arguments[i]->value->location);
                }

                arguments.erase(arguments.begin() + i);

                return;
            }
        }

        visitor->currentScope->block->addInstruction(new StackPushDefault());
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
        if (name == "hold" ||
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
            return info->type;
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

    Define::Define(const SourceLocation location, const std::string name, const std::vector<std::string> inputs, const std::vector<const Token*> instructions) :
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

    VariableInfo::VariableInfo(const unsigned char id, const Token* value) :
        id(id), value(value) {}
    
    FunctionInfo::FunctionInfo(Scope* scope, Type* type) :
        scope(scope), type(type) {}

    InputInfo::InputInfo(const unsigned char id, Type* type) :
        id(id), type(type) {}

    Scope::Scope(const BytecodeTransformer* visitor, Scope* parent, const std::string currentFunction, const std::vector<std::string> inputs) :
        visitor(visitor), parent(parent), currentFunction(currentFunction)
    {
        block = new BytecodeBlock(inputs);

        for (unsigned int i = 0; i < inputs.size(); i++)
        {
            this->inputs[inputs[i]] = new InputInfo(i, nullptr);
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
        if (VariableInfo* info = getVariable(name))
        {
            if (info->value->type(visitor)->checkType(value->type(visitor)))
            {
                return info;
            }

            Utils::parseError("The type \"" + value->type(visitor)->name() + "\" does not match the type \"" + info->value->type(visitor)->name() + "\" of the target variable.", value->location);
        }

        VariableInfo* info = new VariableInfo(variables.size(), value);

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

    FunctionInfo* Scope::addFunction(const std::string name, Scope* scope, Type* type)
    {
        FunctionInfo* info = new FunctionInfo(scope, type);

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

    InputInfo* Scope::setInputType(const std::string name, Type* type)
    {
        if (inputs.count(name))
        {
            inputs[name] = new InputInfo(inputs[name]->id, type);

            return inputs[name];
        }

        return parent->setInputType(name, type);
    }

    bool Scope::checkRecursive(const std::string function) const
    {
        return currentFunction == function || (parent && parent->checkRecursive(function));
    }

    void Scope::checkUses() const
    {
        // add parse locations to all of these if possible?

        for (const std::pair<std::string, VariableInfo*>& pair : variables)
        {
            if (!variablesUsed.count(pair.first))
            {
                Utils::warning("Warning: Unused variable \"" + pair.first + "\".");
            }
        }

        for (const std::pair<std::string, FunctionInfo*>& pair : functions)
        {
            if (!functionsUsed.count(pair.first))
            {
                Utils::warning("Warning: Unused function \"" + pair.first + "\".");
            }
        }

        for (const std::pair<std::string, InputInfo*>& pair : inputs)
        {
            if (!inputsUsed.count(pair.first))
            {
                Utils::warning("Warning: Unused input \"" + pair.first + "\" in function \"" + currentFunction + "\"."); // may be an issue for top level defs
            }
        }
    }

    Program::Program(const std::vector<const Token*> instructions) :
        instructions(instructions) {}

    BytecodeTransformer::BytecodeTransformer(const std::string sourcePath)
    {
        outputPath = std::filesystem::path(sourcePath).replace_extension("obc").string();

        utils = Utils::get();

        currentScope = new Scope(this);
    }

    std::string BytecodeTransformer::transform(const Program* program)
    {
        visit(program);

        return outputPath;
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
            if (!info->type)
            {
                currentScope->setInputType(token->str, expectedType);
            }

            currentScope->block->addInstruction(new GetInput(info->id));
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
            if (!token->values[0]->type(this)->checkType(token->values[i]->type(this)))
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

        if (const InputInfo* info = currentScope->getInput(token->variable))
        {
            currentScope->block->addInstruction(new SetInput(info->id));
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
            for (int i = info->scope->block->inputs.size() - 1; i >= 0; i--)
            {
                if (const Type* inputType = info->scope->getInput(info->scope->block->inputs[i])->type)
                {
                    token->arguments->get(info->scope->block->inputs[i], this, info->scope->getInput(info->scope->block->inputs[i])->type);
                }

                else
                {
                    token->arguments->get(info->scope->block->inputs[i], this);
                }
            }

            token->arguments->confirmEmpty();

            currentScope->block->addInstruction(new CallUser(info->scope->block));

            return;
        }

        bool audioSource = false;

        if (token->name == "copy")
        {
            token->arguments->get("value", this);
        }

        else if (token->name == "time") {}

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
            token->arguments->get("interval", this, new Type(BasicType::Number));
            token->arguments->get("repeats", this, new Type(BasicType::Number));
            token->arguments->get("delay", this, new Type(BasicType::Number));

            currentScope = new Scope(this, currentScope);

            token->arguments->get("function", this); // somehow specify function not something like lfo?

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
        if (token->name == "copy" ||
            token->name == "sine" ||
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

        currentScope->parent->addFunction(token->name, currentScope, token->type(this));

        currentScope = currentScope->parent;
    }

    void BytecodeTransformer::visit(const Program* token)
    {
        resolver->addBlock(currentScope->block);

        for (const Token* instruction : token->instructions)
        {
            instruction->accept(this);
        }

        currentScope->checkUses();

        resolver->output(outputPath);
    }
}
