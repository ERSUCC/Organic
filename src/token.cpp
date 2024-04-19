#include "../include/token.h"

namespace Parser
{
    ParserLocation::ParserLocation(const unsigned int line, const unsigned int character, const unsigned int start, const unsigned int end) :
        line(line), character(character), start(start), end(end) {}

    Token::Token(const ParserLocation location, const std::string str) :
        location(location), str(str) {}

    void Token::accept(BytecodeTransformer* visitor) const {}

    OpenParenthesis::OpenParenthesis(const ParserLocation location) :
        Token(location, "(") {}

    CloseParenthesis::CloseParenthesis(const ParserLocation location) :
        Token(location, ")") {}

    OpenCurlyBracket::OpenCurlyBracket(const ParserLocation location) :
        Token(location, "{") {}

    CloseCurlyBracket::CloseCurlyBracket(const ParserLocation location) :
        Token(location, "}") {}

    Colon::Colon(const ParserLocation location) :
        Token(location, ":") {}

    Comma::Comma(const ParserLocation location) :
        Token(location, ",") {}

    Equals::Equals(const ParserLocation location) :
        Token(location, "=") {}

    Operator::Operator(const ParserLocation location, const std::string str) :
        Token(location, str) {}

    Add::Add(const ParserLocation location) :
        Operator(location, "+") {}

    Subtract::Subtract(const ParserLocation location) :
        Operator(location, "-") {}

    Multiply::Multiply(const ParserLocation location) :
        Operator(location, "*") {}

    Divide::Divide(const ParserLocation location) :
        Operator(location, "/") {}

    DoubleEquals::DoubleEquals(const ParserLocation location) :
        Operator(location, "==") {}

    Less::Less(const ParserLocation location) :
        Operator(location, "<") {}

    Greater::Greater(const ParserLocation location) :
        Operator(location, ">") {}

    LessEqual::LessEqual(const ParserLocation location) :
        Operator(location, "<=") {}

    GreaterEqual::GreaterEqual(const ParserLocation location) :
        Operator(location, ">=") {}

    String::String(const ParserLocation location, const std::string str) :
        Token(location, str) {}

    Value::Value(const ParserLocation location, const std::string str, const double value) :
        Token(location, str), value(value) {}

    void Value::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    NamedConstant::NamedConstant(const ParserLocation location, const std::string constant) :
        Token(location, constant) {}

    void NamedConstant::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Variable::Variable(const ParserLocation location, const std::string variable) :
        Token(location, variable) {}

    void Variable::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Argument::Argument(const ParserLocation location, const std::string name, const Token* value) :
        Token(location, name + ": " + value->str), name(name), value(value) {}

    ArgumentList::ArgumentList(const std::vector<const Argument*> arguments, const std::string name, const std::string path) :
        arguments(arguments), name(name), path(path)
    {
        std::unordered_set<std::string> defined;

        for (const Argument* argument : arguments)
        {
            if (defined.count(argument->name))
            {
                Utils::parseError("Input \"" + argument->name + "\" specified more than once.", path, argument->location.line, argument->location.character);
            }

            defined.insert(argument->name);
        }
    }

    void ArgumentList::get(const std::string name, Token* defaultValue, BytecodeTransformer* visitor)
    {
        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                arguments[i]->value->accept(visitor);

                arguments.erase(arguments.begin() + i);

                return;
            }
        }

        defaultValue->accept(visitor);
    }

    void ArgumentList::confirmEmpty() const
    {
        if (!arguments.empty())
        {
            Utils::parseError("Invalid input name \"" + arguments[0]->name + "\" for function \"" + name + "\".", path, arguments[0]->location.line, arguments[0]->location.character);
        }
    }

    List::List(const ParserLocation location, const std::vector<const Token*> values) :
        Token(location, "()"), values(values) {} // fix str, idk if necessary but should match other structs

    void List::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    AddObject::AddObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " + " + value2->str), value1(value1), value2(value2) {}

    void AddObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    SubtractObject::SubtractObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " - " + value2->str), value1(value1), value2(value2) {}

    void SubtractObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    MultiplyObject::MultiplyObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " * " + value2->str), value1(value1), value2(value2) {}

    void MultiplyObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    DivideObject::DivideObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " / " + value2->str), value1(value1), value2(value2) {}

    void DivideObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    EqualsObject::EqualsObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " == " + value2->str), value1(value1), value2(value2) {}

    void EqualsObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    LessObject::LessObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " < " + value2->str), value1(value1), value2(value2) {}

    void LessObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    GreaterObject::GreaterObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " > " + value2->str), value1(value1), value2(value2) {}

    void GreaterObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    LessEqualObject::LessEqualObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " <= " + value2->str), value1(value1), value2(value2) {}

    void LessEqualObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    GreaterEqualObject::GreaterEqualObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " >= " + value2->str), value1(value1), value2(value2) {}

    void GreaterEqualObject::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Instruction::Instruction(const ParserLocation location, const std::string str) :
        Token(location, str) {}

    Assign::Assign(const ParserLocation location, const std::string variable, const Token* value) :
        Instruction(location, variable + " = " + value->str), variable(variable), value(value) {}

    void Assign::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Call::Call(const ParserLocation location, const std::string name, ArgumentList* arguments) :
        Instruction(location, name + "()"), name(name), arguments(arguments) {} // fix str, same as list

    void Call::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    CodeBlock::CodeBlock(const ParserLocation location, const std::vector<const Instruction*> instructions) :
        Token(location, ""), instructions(instructions) {} // fix str, same as list

    void CodeBlock::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Scope::Scope(Scope* parent) :
        parent(parent) {}

    bool Scope::getVariable(const std::string name)
    {
        if (variables.count(name))
        {
            variablesUsed.insert(name);

            return true;
        }

        if (parent && parent->getVariable(name))
        {
            return true;
        }

        return false;
    }

    void Scope::addVariable(const std::string name)
    {
        if (!getVariable(name))
        {
            variables.insert(name);
        }
    }

    void Scope::checkVariableUses() const
    {
        for (const std::string variable : variables)
        {
            if (!variablesUsed.count(variable))
            {
                Utils::warning("Warning: Unused variable \"" + variable + "\".");
            }
        }
    }

    Program::Program(const std::vector<const Instruction*> instructions) :
        Token(ParserLocation(0, 0, 0, 0), ""), instructions(instructions) {} // fix later, same as list

    void Program::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    BytecodeTransformer::BytecodeTransformer(const std::string path) :
        sourcePath(path)
    {
        outputPath = std::filesystem::path(path).replace_extension("obc").string();

        currentScope = new Scope();
    }

    std::string BytecodeTransformer::transform(const Program* program)
    {
        visit(program);

        return outputPath;
    }

    void BytecodeTransformer::visit(const Value* token)
    {
        currentScope->block->instructions.push_back(new StackPushDouble(token->value));
    }

    void BytecodeTransformer::visit(const NamedConstant* token)
    {
        if (token->str == "sequence-forwards")
        {
            currentScope->block->instructions.push_back(new StackPushByte(0x00));
        }

        else if (token->str == "sequence-backwards")
        {
            currentScope->block->instructions.push_back(new StackPushByte(0x01));
        }

        else if (token->str == "sequence-ping-pong")
        {
            currentScope->block->instructions.push_back(new StackPushByte(0x02));
        }

        else if (token->str == "sequence-random")
        {
            currentScope->block->instructions.push_back(new StackPushByte(0x03));
        }

        else if (token->str == "random-step")
        {
            currentScope->block->instructions.push_back(new StackPushByte(0x00));
        }

        else if (token->str == "random-linear")
        {
            currentScope->block->instructions.push_back(new StackPushByte(0x01));
        }
    }

    void BytecodeTransformer::visit(const Variable* token)
    {
        std::string name = token->str;

        bool copy = false;

        if (name[0] == '#')
        {
            name = name.substr(1);

            copy = true;
        }

        if (currentScope->getVariable(name))
        {
            if (currentVariable == name)
            {
                return Utils::parseError("Variable \"" + name + "\" referenced in its own definition.", sourcePath, token->location.line, token->location.character);
            }

            if (copy)
            {
                currentScope->block->instructions.push_back(new GetVariableCopy(name));
            }

            else
            {
                currentScope->block->instructions.push_back(new GetVariable(name));
            }
        }

        else
        {
            Utils::parseError("Unrecognized variable name \"" + name + "\".", sourcePath, token->location.line, token->location.character);
        }
    }

    void BytecodeTransformer::visit(const List* token)
    {
        for (int i = token->values.size() - 1; i >= 0; i--)
        {
            token->values[i]->accept(this);
        }

        currentScope->block->instructions.push_back(new StackPushInt(token->values.size()));
        currentScope->block->instructions.push_back(new CallNative("list"));
    }

    void BytecodeTransformer::visit(const AddObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("add"));
    }

    void BytecodeTransformer::visit(const SubtractObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("subtract"));
    }

    void BytecodeTransformer::visit(const MultiplyObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("multiply"));
    }

    void BytecodeTransformer::visit(const DivideObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("divide"));
    }

    void BytecodeTransformer::visit(const EqualsObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("equal"));
    }

    void BytecodeTransformer::visit(const LessObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("less"));
    }

    void BytecodeTransformer::visit(const GreaterObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("greater"));
    }

    void BytecodeTransformer::visit(const LessEqualObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("lessequal"));
    }

    void BytecodeTransformer::visit(const GreaterEqualObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->instructions.push_back(new CallNative("greaterequal"));
    }

    void BytecodeTransformer::visit(const Assign* token)
    {
        currentVariable = token->variable;

        currentScope->addVariable(currentVariable);

        token->value->accept(this);

        currentScope->block->instructions.push_back(new SetVariable(currentVariable));

        currentVariable = ""; // this breaks for multi-nested scopes, fix with stack
    }

    void BytecodeTransformer::visit(const Call* token)
    {
        const std::string name = token->name;

        if (name == "sine")
        {
            token->arguments->get("frequency", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("effects", new List(ParserLocation(0, 0, 0, 0), std::vector<const Token*>()), this);
            token->arguments->get("pan", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("volume", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "square")
        {
            token->arguments->get("frequency", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("effects", new List(ParserLocation(0, 0, 0, 0), std::vector<const Token*>()), this);
            token->arguments->get("pan", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("volume", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "saw")
        {
            token->arguments->get("frequency", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("effects", new List(ParserLocation(0, 0, 0, 0), std::vector<const Token*>()), this);
            token->arguments->get("pan", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("volume", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "triangle")
        {
            token->arguments->get("frequency", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("effects", new List(ParserLocation(0, 0, 0, 0), std::vector<const Token*>()), this);
            token->arguments->get("pan", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("volume", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "noise")
        {
            token->arguments->get("effects", new List(ParserLocation(0, 0, 0, 0), std::vector<const Token*>()), this);
            token->arguments->get("pan", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("volume", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "hold")
        {
            token->arguments->get("length", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("value", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "lfo")
        {
            token->arguments->get("length", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("to", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("from", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "sweep")
        {
            token->arguments->get("length", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("to", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("from", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "sequence")
        {
            token->arguments->get("order", new NamedConstant(ParserLocation(0, 0, 0, 0), "sequence-forwards"), this);
            token->arguments->get("values", new List(ParserLocation(0, 0, 0, 0), std::vector<const Token*>()), this);
        }

        else if (name == "repeat")
        {
            token->arguments->get("repeats", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("value", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "random")
        {
            token->arguments->get("type", new NamedConstant(ParserLocation(0, 0, 0, 0), "random-step"), this);
            token->arguments->get("length", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("to", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("from", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "limit")
        {
            token->arguments->get("max", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("min", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("value", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "trigger")
        {
            token->arguments->get("value", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("condition", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "delay")
        {
            token->arguments->get("feedback", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("delay", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
            token->arguments->get("mix", new Value(ParserLocation(0, 0, 0, 0), "0", 0), this);
        }

        else if (name == "lowpass")
        {
            // stuff idk haven't solidified yet
        }

        else if (name == "perform")
        {
            token->arguments->get("function", nullptr, this);
        }

        else
        {
            return Utils::parseError("Unknown function name \"" + name + "\".", sourcePath, token->location.line, token->location.character);
        }

        token->arguments->confirmEmpty();

        currentScope->block->instructions.push_back(new CallNative(name));
    }

    void BytecodeTransformer::visit(const CodeBlock* token)
    {
        Scope* scope = new Scope(currentScope);

        currentScope = scope;

        for (const Instruction* instruction : token->instructions)
        {
            instruction->accept(this);
        }

        currentScope->checkVariableUses();

        resolver->blocks.push_back(currentScope->block);

        currentScope = currentScope->parent;
    }

    void BytecodeTransformer::visit(const Program* token)
    {
        resolver->blocks.push_back(currentScope->block);

        for (const Instruction* instruction : token->instructions)
        {
            instruction->accept(this);
        }

        currentScope->checkVariableUses();

        resolver->output(outputPath);
    }
}
