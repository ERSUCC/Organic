#include "../include/token.h"

namespace Parser
{
    ParserLocation::ParserLocation(const unsigned int line, const unsigned int character, const unsigned int start, const unsigned int end) :
        line(line), character(character), start(start), end(end) {}

    Token::Token(const ParserLocation location, const std::string str) :
        location(location), str(str) {}

    void Token::accept(BytecodeTransformer* visitor) const {}

    std::string listToString(const std::vector<const Token*> list)
    {
        std::string result = "(";

        if (list.size() > 0)
        {
            result += list[0]->str;

            for (unsigned int i = 1; i < list.size(); i++)
            {
                result += ", " + list[i]->str;
            }
        }

        return result + ")";
    }

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

    Power::Power(const ParserLocation location) :
        Operator(location, "^") {}

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
    
    std::string argumentsToString(const std::vector<const Argument*> arguments)
    {
        std::string result = "(";

        if (arguments.size() > 0)
        {
            result += arguments[0]->str;

            for (unsigned int i = 1; i < arguments.size(); i++)
            {
                result += ", " + arguments[i]->str;
            }
        }

        return result + ")";
    }

    ArgumentList::ArgumentList(const std::vector<const Argument*> arguments, const std::string name, const std::string path) :
        arguments(arguments), name(name), path(path), str(argumentsToString(arguments))
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

    void ArgumentList::get(const std::string name, BytecodeTransformer* visitor)
    {
        count++;

        for (unsigned int i = 0; i < arguments.size(); i++)
        {
            if (arguments[i]->name == name)
            {
                arguments[i]->value->accept(visitor);

                arguments.erase(arguments.begin() + i);

                return;
            }
        }

        (new Value(ParserLocation(0, 0, 0, 0), "0", 0))->accept(visitor);
    }

    void ArgumentList::confirmEmpty() const
    {
        if (!arguments.empty())
        {
            Utils::parseError("Invalid input name \"" + arguments[0]->name + "\" for function \"" + name + "\".", path, arguments[0]->location.line, arguments[0]->location.character);
        }
    }

    List::List(const ParserLocation location, const std::vector<const Token*> values) :
        Token(location, listToString(values)), values(values) {}

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

    PowerObject::PowerObject(const ParserLocation location, const Token* value1, const Token* value2) :
        Token(location, value1->str + " ^ " + value2->str), value1(value1), value2(value2) {}

    void PowerObject::accept(BytecodeTransformer* visitor) const
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

    ParenthesizedExpression::ParenthesizedExpression(const ParserLocation location, const Token* value) :
        Token(location, "(" + value->str + ")"), value(value) {}
    
    void ParenthesizedExpression::accept(BytecodeTransformer* visitor) const
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
        Instruction(location, name + arguments->str), name(name), arguments(arguments) {}

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

    Define::Define(const ParserLocation location, const std::string name, const std::vector<std::string> inputs, const CodeBlock* body) :
        Token(location, ""), name(name), inputs(inputs), body(body) {} // fix str, same as list

    void Define::accept(BytecodeTransformer* visitor) const
    {
        visitor->visit(this);
    }

    Scope::Scope(Scope* parent, const std::vector<std::string> inputs) :
        parent(parent)
    {
        block = new BytecodeBlock(inputs);

        for (unsigned int i = 0; i < inputs.size(); i++)
        {
            this->inputs[inputs[i]] = i;
        }
    }

    std::optional<unsigned char> Scope::getVariable(const std::string name)
    {
        if (variables.count(name))
        {
            variablesUsed.insert(name);

            return variables[name];
        }

        if (parent)
        {
            if (std::optional<unsigned char> result = parent->getVariable(name))
            {
                return result;
            }
        }

        return std::nullopt;
    }

    void Scope::addVariable(const std::string name)
    {
        if (!getVariable(name))
        {
            variables[name] = variables.size();
        }
    }

    BytecodeBlock* Scope::getFunction(const std::string name)
    {
        if (functions.count(name))
        {
            functionsUsed.insert(name);

            return functions[name];
        }

        if (parent)
        {
            if (BytecodeBlock* function = parent->getFunction(name))
            {
                return function;
            }
        }

        return nullptr;
    }

    void Scope::addFunction(const std::string name, BytecodeBlock* body)
    {
        functions[name] = body;
    }

    std::optional<unsigned char> Scope::getInput(const std::string input)
    {
        if (inputs.count(input))
        {
            inputsUsed.insert(input);

            return inputs[input];
        }

        if (parent)
        {
            if (std::optional<unsigned char> result = parent->getInput(input))
            {
                return result;
            }
        }

        return std::nullopt;
    }

    void Scope::checkUses() const
    {
        // add parse locations to all of these if possible?

        for (const std::pair<std::string, unsigned char>& pair : variables)
        {
            if (!variablesUsed.count(pair.first))
            {
                Utils::warning("Warning: Unused variable \"" + pair.first + "\".");
            }
        }

        for (const std::pair<std::string, BytecodeBlock*>& pair : functions)
        {
            if (!functionsUsed.count(pair.first))
            {
                Utils::warning("Warning: Unused function \"" + pair.first + "\".");
            }
        }

        for (const std::pair<std::string, unsigned char>& pair : inputs)
        {
            if (!inputsUsed.count(pair.first))
            {
                Utils::warning("Warning: Unused input \"" + pair.first + "\"."); // add function name to be more clear
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

        utils = Utils::get();

        currentScope = new Scope();
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
        std::string name = token->str;

        if (const std::optional<unsigned char> input = currentScope->getInput(name))
        {
            currentScope->block->addInstruction(new GetInput(input.value()));
        }

        else if (const std::optional<unsigned char> result = currentScope->getVariable(name))
        {
            currentScope->block->addInstruction(new GetVariable(result.value()));
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

        currentScope->block->addInstruction(new StackPushInt(token->values.size()));
        currentScope->block->addInstruction(new CallNative("list", 1));
    }

    void BytecodeTransformer::visit(const AddObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("add", 2));
    }

    void BytecodeTransformer::visit(const SubtractObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("subtract", 2));
    }

    void BytecodeTransformer::visit(const MultiplyObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("multiply", 2));
    }

    void BytecodeTransformer::visit(const DivideObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("divide", 2));
    }

    void BytecodeTransformer::visit(const PowerObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("power", 2));
    }

    void BytecodeTransformer::visit(const EqualsObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("equal", 2));
    }

    void BytecodeTransformer::visit(const LessObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("less", 2));
    }

    void BytecodeTransformer::visit(const GreaterObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("greater", 2));
    }

    void BytecodeTransformer::visit(const LessEqualObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("lessequal", 2));
    }

    void BytecodeTransformer::visit(const GreaterEqualObject* token)
    {
        token->value2->accept(this);
        token->value1->accept(this);

        currentScope->block->addInstruction(new CallNative("greaterequal", 2));
    }

    void BytecodeTransformer::visit(const ParenthesizedExpression* token)
    {
        token->value->accept(this);
    }

    void BytecodeTransformer::visit(const Assign* token)
    {
        token->value->accept(this);

        if (std::optional<unsigned char> input = currentScope->getInput(token->variable))
        {
            currentScope->block->addInstruction(new SetInput(input.value()));
        }

        else
        {
            currentScope->addVariable(token->variable);

            currentScope->block->addInstruction(new SetVariable(currentScope->getVariable(token->variable).value()));
        }
    }

    void BytecodeTransformer::visit(const Call* token)
    {
        const std::string name = token->name;

        if (const BytecodeBlock* function = currentScope->getFunction(name))
        {
            for (int i = function->inputs.size() - 1; i >= 0; i--)
            {
                token->arguments->get(function->inputs[i], this);
            }

            token->arguments->confirmEmpty();

            currentScope->block->addInstruction(new CallUser(function, function->inputs.size()));

            return;
        }

        if (name == "copy")
        {
            token->arguments->get("value", this);
        }

        else if (name == "sine")
        {
            token->arguments->get("frequency", this);
            token->arguments->get("effects", this);
            token->arguments->get("pan", this);
            token->arguments->get("volume", this);
        }

        else if (name == "square")
        {
            token->arguments->get("frequency", this);
            token->arguments->get("effects", this);
            token->arguments->get("pan", this);
            token->arguments->get("volume", this);
        }

        else if (name == "saw")
        {
            token->arguments->get("frequency", this);
            token->arguments->get("effects", this);
            token->arguments->get("pan", this);
            token->arguments->get("volume", this);
        }

        else if (name == "triangle")
        {
            token->arguments->get("frequency", this);
            token->arguments->get("effects", this);
            token->arguments->get("pan", this);
            token->arguments->get("volume", this);
        }

        else if (name == "noise")
        {
            token->arguments->get("effects", this);
            token->arguments->get("pan", this);
            token->arguments->get("volume", this);
        }

        else if (name == "hold")
        {
            token->arguments->get("length", this);
            token->arguments->get("value", this);
        }

        else if (name == "lfo")
        {
            token->arguments->get("length", this);
            token->arguments->get("to", this);
            token->arguments->get("from", this);
        }

        else if (name == "sweep")
        {
            token->arguments->get("length", this);
            token->arguments->get("to", this);
            token->arguments->get("from", this);
        }

        else if (name == "sequence")
        {
            token->arguments->get("order", this);
            token->arguments->get("values", this);
        }

        else if (name == "repeat")
        {
            token->arguments->get("repeats", this);
            token->arguments->get("value", this);
        }

        else if (name == "random")
        {
            token->arguments->get("type", this);
            token->arguments->get("length", this);
            token->arguments->get("to", this);
            token->arguments->get("from", this);
        }

        else if (name == "limit")
        {
            token->arguments->get("max", this);
            token->arguments->get("min", this);
            token->arguments->get("value", this);
        }

        else if (name == "trigger")
        {
            token->arguments->get("value", this);
            token->arguments->get("condition", this);
        }

        else if (name == "if")
        {
            token->arguments->get("false", this);
            token->arguments->get("true", this);
            token->arguments->get("condition", this);
        }

        else if (name == "delay")
        {
            token->arguments->get("feedback", this);
            token->arguments->get("delay", this);
            token->arguments->get("mix", this);
        }

        else if (name == "lowpass")
        {
            // stuff idk haven't solidified yet
        }

        else if (name == "perform")
        {
            token->arguments->get("delay", this);

            currentScope = new Scope(currentScope);

            token->arguments->get("function", this);

            BytecodeBlock* block = currentScope->block;

            resolver->addBlock(block);

            currentScope = currentScope->parent;

            currentScope->block->addInstruction(new StackPushAddress(block));
        }

        else
        {
            return Utils::parseError("Unknown function name \"" + name + "\".", sourcePath, token->location.line, token->location.character);
        }

        token->arguments->confirmEmpty();

        currentScope->block->addInstruction(new CallNative(name, token->arguments->count));
    }

    void BytecodeTransformer::visit(const CodeBlock* token)
    {
        for (const Instruction* instruction : token->instructions)
        {
            instruction->accept(this);
        }
    }

    void BytecodeTransformer::visit(const Define* token)
    {
        if (token->name == "copy" ||
            token->name == "sine" ||
            token->name == "square" ||
            token->name == "saw" ||
            token->name == "triangle" ||
            token->name == "noise" ||
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
            token->name == "perform" ||
            currentScope->getFunction(token->name))
        {
            return Utils::parseError("A function already exists with the name \"" + token->name + "\".", sourcePath, token->location.line, token->location.character);
        }

        currentScope = new Scope(currentScope, token->inputs);

        token->body->accept(this);

        currentScope->checkUses();

        BytecodeBlock* block = currentScope->block;

        resolver->addBlock(block);

        currentScope = currentScope->parent;

        currentScope->addFunction(token->name, block);
    }

    void BytecodeTransformer::visit(const Program* token)
    {
        resolver->addBlock(currentScope->block);

        for (const Instruction* instruction : token->instructions)
        {
            instruction->accept(this);
        }

        currentScope->checkUses();

        resolver->output(outputPath);
    }
}
