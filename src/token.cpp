#include "../include/token.h"

namespace Parser
{
    ReturnType::ReturnType(const BasicReturnType primaryType, ReturnType* subType) :
        primaryType(primaryType), subType(subType) {}
    
    std::string ReturnType::getTypeName() const
    {
        switch (primaryType)
        {
            case BasicReturnType::None:
                return "nothing";
            
            case BasicReturnType::Any:
                return "anything";
            
            case BasicReturnType::SequenceOrder:
                return "sequence order constant";
            
            case BasicReturnType::RandomType:
                return "random type constant";
            
            case BasicReturnType::Number:
                return "number";
            
            case BasicReturnType::Boolean:
                return "boolean";
            
            case BasicReturnType::List:
                return "list of " + subType->getTypeName();
            
            case BasicReturnType::AudioSource:
                return "audio source";
            
            case BasicReturnType::Effect:
                return "effect";
        }

        return "";
    }
    
    bool ReturnType::checkType(const ReturnType* other) const
    {
        if (!other)
        {
            return false;
        }

        switch (primaryType)
        {
            case BasicReturnType::None:
                if (other->primaryType != BasicReturnType::None)
                {
                    return false;
                }

                break;
            
            case BasicReturnType::Any:
                break;
            
            case BasicReturnType::SequenceOrder:
                if (other->primaryType != BasicReturnType::SequenceOrder)
                {
                    return false;
                }

                break;
            
            case BasicReturnType::RandomType:
                if (other->primaryType != BasicReturnType::RandomType)
                {
                    return false;
                }

                break;
            
            case BasicReturnType::Number:
                if (other->primaryType != BasicReturnType::Number)
                {
                    return false;
                }

                break;
            
            case BasicReturnType::Boolean:
                if (other->primaryType != BasicReturnType::Boolean)
                {
                    return false;
                }

                break;

            case BasicReturnType::List:
                if (other->primaryType != BasicReturnType::List)
                {
                    return false;
                }

                break;

            case BasicReturnType::AudioSource:
                if (other->primaryType != BasicReturnType::AudioSource)
                {
                    return false;
                }

                break;

            case BasicReturnType::Effect:
                if (other->primaryType != BasicReturnType::Effect)
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

    Token::Token(const SourceLocation location) :
        location(location) {}

    ReturnType* Token::returnType(const BytecodeTransformer* visitor) const
    {
        return new ReturnType(BasicReturnType::Any);
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

    ReturnType* Value::returnType(const BytecodeTransformer* visitor) const
    {
        return new ReturnType(BasicReturnType::Number);
    }

    void Value::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    NamedConstant::NamedConstant(const SourceLocation location, const std::string constant) :
        BasicToken(location, constant) {}
    
    ReturnType* NamedConstant::returnType(const BytecodeTransformer* visitor) const
    {
        if (str == "sequence-forwards" || str == "sequence-backwards" || str == "sequence-ping-pong" || str == "sequence-random")
        {
            return new ReturnType(BasicReturnType::SequenceOrder);
        }

        if (str == "random-step" || str == "random-linear")
        {
            return new ReturnType(BasicReturnType::RandomType);
        }

        if (str == "pi" || str == "e")
        {
            return new ReturnType(BasicReturnType::Number);
        }

        return new ReturnType(BasicReturnType::None);
    }

    void NamedConstant::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Variable::Variable(const SourceLocation location, const std::string variable) :
        BasicToken(location, variable) {}

    ReturnType* Variable::returnType(const BytecodeTransformer* visitor) const
    {
        if (VariableInfo* info = visitor->currentScope->getVariable(str))
        {
            return info->value->returnType(visitor);
        }

        else if (InputInfo* info = visitor->currentScope->getInput(str))
        {
            return info->returnType;
        }

        Utils::parseError("Unrecognized variable name \"" + str + "\".", location);

        return new ReturnType(BasicReturnType::None);
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

    void ArgumentList::get(const std::string name, BytecodeTransformer* visitor, ReturnType* expectedType)
    {
        count++;

        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                visitor->expectedType = expectedType;

                arguments[i]->value->accept(visitor);

                const ReturnType* argumentType = arguments[i]->value->returnType(visitor);

                if (!expectedType->checkType(argumentType))
                {
                    Utils::parseError("Expected \"" + expectedType->getTypeName() + "\", received \"" + argumentType->getTypeName() + "\".", arguments[i]->value->location);
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

    ReturnType* List::returnType(const BytecodeTransformer* visitor) const
    {
        return new ReturnType(BasicReturnType::List, values[0]->returnType(visitor));
    }

    void List::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    ParenthesizedExpression::ParenthesizedExpression(const SourceLocation location, const Token* value) :
        Token(location), value(value) {}
    
    ReturnType* ParenthesizedExpression::returnType(const BytecodeTransformer* visitor) const
    {
        return value->returnType(visitor);
    }
    
    void ParenthesizedExpression::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Assign::Assign(const SourceLocation location, const std::string variable, const Token* value) :
        Token(location), variable(variable), value(value) {}

    void Assign::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Call::Call(const SourceLocation location, const std::string name, ArgumentList* arguments) :
        Token(location), name(name), arguments(arguments) {}
    
    ReturnType* Call::returnType(const BytecodeTransformer* visitor) const
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
            return new ReturnType(BasicReturnType::Number);
        }

        if (name == "sine" ||
            name == "square" ||
            name == "triangle" ||
            name == "saw" ||
            name == "noise" ||
            name == "blend")
        {
            return new ReturnType(BasicReturnType::AudioSource);
        }

        if (name == "delay" ||
            name == "lowpass")
        {
            return new ReturnType(BasicReturnType::Effect);
        }

        if (FunctionInfo* info = visitor->currentScope->getFunction(name))
        {
            return info->returnType;
        }

        return new ReturnType(BasicReturnType::None);
    }
    
    void Call::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    CallAlias::CallAlias(const SourceLocation location, const std::string name, const std::vector<const Token*>& arguments) :
        Call(location, name, ArgumentList::constructAlias(arguments, name)) {}
    
    ReturnType* CallAlias::returnType(const BytecodeTransformer* visitor) const
    {
        if (name == "add" ||
            name == "subtract" ||
            name == "multiply" ||
            name == "divide" ||
            name == "power")
        {
            return new ReturnType(BasicReturnType::Number);
        }

        else if (name == "equal" ||
                 name == "less" ||
                 name == "greater" ||
                 name == "lessequal" ||
                 name == "greaterequal")
        {
            return new ReturnType(BasicReturnType::Boolean);
        }

        return new ReturnType(BasicReturnType::None);
    }

    void CallAlias::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Define::Define(const SourceLocation location, const std::string name, const std::vector<std::string> inputs, const std::vector<const Token*> instructions) :
        Token(location), name(name), inputs(inputs), instructions(instructions) {}

    ReturnType* Define::returnType(const BytecodeTransformer* visitor) const
    {
        if (instructions.empty())
        {
            return new ReturnType(BasicReturnType::None);
        }

        return instructions.back()->returnType(visitor);
    }
    
    void Define::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    VariableInfo::VariableInfo(const unsigned char id, const Token* value) :
        id(id), value(value) {}
    
    FunctionInfo::FunctionInfo(Scope* scope, ReturnType* returnType) :
        scope(scope), returnType(returnType) {}

    InputInfo::InputInfo(const unsigned char id, ReturnType* returnType) :
        id(id), returnType(returnType) {}

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
            if (info->value->returnType(visitor)->checkType(value->returnType(visitor)))
            {
                return info;
            }

            Utils::parseError("The type \"" + value->returnType(visitor)->getTypeName() + "\" does not match the type \"" + info->value->returnType(visitor)->getTypeName() + "\" of the target variable.", value->location);
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

    FunctionInfo* Scope::addFunction(const std::string name, Scope* scope, ReturnType* returnType)
    {
        FunctionInfo* info = new FunctionInfo(scope, returnType);

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

    InputInfo* Scope::setInputType(const std::string name, ReturnType* returnType)
    {
        if (inputs.count(name))
        {
            inputs[name] = new InputInfo(inputs[name]->id, returnType);

            return inputs[name];
        }

        return parent->setInputType(name, returnType);
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
            if (!info->returnType)
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
        ReturnType* innerType = expectedType->subType;

        for (int i = token->values.size() - 1; i >= 0; i--)
        {
            expectedType = innerType;

            token->values[i]->accept(this);
        }

        for (unsigned int i = 1; i < token->values.size(); i++)
        {
            if (!token->values[0]->returnType(this)->checkType(token->values[i]->returnType(this)))
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
                if (const ReturnType* inputType = info->scope->getInput(info->scope->block->inputs[i])->returnType)
                {
                    token->arguments->get(info->scope->block->inputs[i], this, info->scope->getInput(info->scope->block->inputs[i])->returnType);
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
            token->arguments->get("frequency", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("effects", this, new ReturnType(BasicReturnType::List, new ReturnType(BasicReturnType::Effect)));
            token->arguments->get("pan", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("volume", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "noise")
        {
            token->arguments->get("effects", this, new ReturnType(BasicReturnType::List, new ReturnType(BasicReturnType::Effect)));
            token->arguments->get("pan", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("volume", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "blend")
        {
            token->arguments->get("position", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("sources", this, new ReturnType(BasicReturnType::List, new ReturnType(BasicReturnType::AudioSource)));
        }

        else if (token->name == "hold")
        {
            token->arguments->get("length", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("value", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "lfo")
        {
            token->arguments->get("length", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("to", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("from", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "sweep")
        {
            token->arguments->get("length", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("to", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("from", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "sequence")
        {
            token->arguments->get("order", this, new ReturnType(BasicReturnType::SequenceOrder));
            token->arguments->get("values", this, new ReturnType(BasicReturnType::List, new ReturnType(BasicReturnType::Number)));
        }

        else if (token->name == "repeat")
        {
            token->arguments->get("repeats", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("value", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "random")
        {
            token->arguments->get("type", this, new ReturnType(BasicReturnType::RandomType));
            token->arguments->get("length", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("to", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("from", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "limit")
        {
            token->arguments->get("max", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("min", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("value", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "trigger")
        {
            token->arguments->get("value", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("condition", this, new ReturnType(BasicReturnType::Boolean));
        }

        else if (token->name == "if")
        {
            token->arguments->get("false", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("true", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("condition", this, new ReturnType(BasicReturnType::Boolean));
        }

        else if (token->name == "delay")
        {
            token->arguments->get("feedback", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("delay", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("mix", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "play")
        {
            token->arguments->get("source", this, new ReturnType(BasicReturnType::AudioSource));
        }

        else if (token->name == "perform")
        {
            token->arguments->get("interval", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("repeats", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("delay", this, new ReturnType(BasicReturnType::Number));

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
    }

    void BytecodeTransformer::visit(const CallAlias* token)
    {
        if (token->name == "add")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "subtract")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "multiply")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "divide")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "power")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "equal")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "less")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "greater")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "lessequal")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
        }

        else if (token->name == "greaterequal")
        {
            token->arguments->get("1", this, new ReturnType(BasicReturnType::Number));
            token->arguments->get("0", this, new ReturnType(BasicReturnType::Number));
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

        currentScope->parent->addFunction(token->name, currentScope, token->returnType(this));

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
