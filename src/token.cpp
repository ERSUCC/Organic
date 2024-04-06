#include "../include/token.h"

Token::Token(const int line, const int character, const std::string str) :
    line(line), character(character), str(str) {}

std::string Token::string() const
{
    return str;
}

void Token::accept(BytecodeTransformer* visitor) const {}

TokenRange::TokenRange(const int start, const int end, Token* token) :
    start(start), end(end), token(token) {}

OpenParenthesis::OpenParenthesis(const int line, const int character) :
    Token(line, character, "(") {}

CloseParenthesis::CloseParenthesis(const int line, const int character) :
    Token(line, character, ")") {}

OpenCurlyBracket::OpenCurlyBracket(const int line, const int character) :
    Token(line, character, "{") {}

CloseCurlyBracket::CloseCurlyBracket(const int line, const int character) :
    Token(line, character, "}") {}

Colon::Colon(const int line, const int character) :
    Token(line, character, ":") {}

Comma::Comma(const int line, const int character) :
    Token(line, character, ",") {}

Equals::Equals(const int line, const int character) :
    Token(line, character, "=") {}

Operator::Operator(const int line, const int character, const std::string str) :
    Token(line, character, str) {}

AddToken::AddToken(const int line, const int character) :
    Operator(line, character, "+") {}

SubtractToken::SubtractToken(const int line, const int character) :
    Operator(line, character, "-") {}

MultiplyToken::MultiplyToken(const int line, const int character) :
    Operator(line, character, "*") {}

DivideToken::DivideToken(const int line, const int character) :
    Operator(line, character, "/") {}

Name::Name(const int line, const int character, const std::string name, const bool value) :
    Token(line, character, name), name(name), value(value) {}

void Name::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

Constant::Constant(const int line, const int character, const std::string str) :
    Token(line, character, str), value(std::stod(str)) {}

void Constant::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

Argument::Argument(const int line, const int character, const std::string name, const Token* value) :
    Token(line, character, name + ": " + value->string()), name(name), value(value) {}

ArgumentList::ArgumentList(const std::vector<Argument*> arguments, const std::string name, const std::string path) :
    arguments(arguments), name(name), path(path)
{
    std::unordered_set<std::string> defined;

    for (Argument* argument : arguments)
    {
        if (defined.count(argument->name))
        {
            Utils::parseError("Input \"" + argument->name + "\" specified more than once.", path, argument->line, argument->character);
        }

        defined.insert(argument->name);
    }
}

void ArgumentList::get(const std::string name, Token* defaultValue, BytecodeTransformer* visitor)
{
    int i = 0;

    while (i < arguments.size() && arguments[i]->name != name)
    {
        i++;
    }

    if (i < arguments.size())
    {
        arguments[i]->value->accept(visitor);

        arguments.erase(arguments.begin() + i);
    }

    else
    {
        defaultValue->accept(visitor);
    }
}

void ArgumentList::confirmEmpty() const
{
    if (!arguments.empty())
    {
        Utils::parseError("Invalid input name \"" + arguments[0]->name + "\" for function \"" + name + "\".", path, arguments[0]->line, arguments[0]->character);
    }
}

std::string ArgumentList::string() const
{
    if (arguments.empty())
    {
        return "";
    }

    std::string result = arguments[0]->string();

    for (int i = 1; i < arguments.size(); i++)
    {
        result += ", " + arguments[i]->string();
    }

    return result;
}

ListToken::ListToken(const int line, const int character, const std::vector<Token*> values) :
    Token(line, character), values(values) {}

std::string ListToken::string() const
{
    if (values.size() == 0)
    {
        return "()";
    }

    std::string result = "(" + values[0]->string();

    for (int i = 1; i < values.size(); i++)
    {
        result += ", " + values[i]->string();
    }

    return result + ")";
}

void ListToken::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

Combine::Combine(const Token* value1, const Token* value2, const std::string op) :
    Token(value1->line, value1->character, value1->string() + " " + op + " " + value2->string()), value1(value1), value2(value2) {}

Add::Add(const Token* value1, const Token* value2) :
    Combine(value1, value2, "+") {}

void Add::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

Subtract::Subtract(const Token* value1, const Token* value2) :
    Combine(value1, value2, "-") {}

void Subtract::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

Multiply::Multiply(const Token* value1, const Token* value2) :
    Combine(value1, value2, "*") {}

void Multiply::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

Divide::Divide(const Token* value1, const Token* value2) :
    Combine(value1, value2, "/") {}

void Divide::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

Instruction::Instruction(const int line, const int character, const std::string str) :
    Token(line, character, str) {}

Assign::Assign(const Name* variable, const Token* value) :
    Instruction(variable->line, variable->character, variable->string() + " = " + value->string()), variable(variable), value(value) {}

void Assign::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

Call::Call(const Name* name, ArgumentList* arguments) :
    Instruction(name->line, name->character), name(name), arguments(arguments) {}

std::string Call::string() const
{
    return name->name + "(" + arguments->string() + ")";
}

void Call::accept(BytecodeTransformer* visitor) const
{
    visitor->visit(this);
}

CodeBlock::CodeBlock(const int line, const int character, const std::vector<Instruction*> instructions) :
    Token(line, character), instructions(instructions) {}

std::string CodeBlock::string() const
{
    std::string result = "{\n";

    for (const Instruction* instruction : instructions)
    {
        result += "\t" + instruction->string() + "\n";
    }

    return result + "}";
}

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

Program::Program(const std::vector<Instruction*> instructions) :
    Token(0, 0), instructions(instructions) {}

std::string Program::string() const
{
    if (instructions.size() == 0)
    {
        return "";
    }

    std::string result = instructions[0]->string();

    for (int i = 1; i < instructions.size(); i++)
    {
        result += "\n" + instructions[i]->string();
    }

    return result;
}

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

void BytecodeTransformer::visit(const Name* token)
{
    if (currentScope->getVariable(token->name))
    {
        if (currentVariable == token->name)
        {
            return Utils::parseError("Variable \"" + token->name + "\" referenced in its own definition.", sourcePath, token->line, token->character);
        }

        if (token->value)
        {
            currentScope->block->instructions.push_back(new GetVariableCopy(token->name));
        }

        else
        {
            currentScope->block->instructions.push_back(new GetVariable(token->name));
        }

        return;
    }

    if (token->value)
    {
        return Utils::parseError("\"#\" can only precede variable names.", sourcePath, token->line, token->character);
    }

    // make sure to check constants earlier because here it could get mixed up with the numbers

    if (token->name == "sequence-forwards")
    {
        currentScope->block->instructions.push_back(new StackPushByte(0x00));
    }

    else if (token->name == "sequence-backwards")
    {
        currentScope->block->instructions.push_back(new StackPushByte(0x01));
    }

    else if (token->name == "sequence-ping-pong")
    {
        currentScope->block->instructions.push_back(new StackPushByte(0x02));
    }

    else if (token->name == "sequence-random")
    {
        currentScope->block->instructions.push_back(new StackPushByte(0x03));
    }

    else if (token->name == "random-step")
    {
        currentScope->block->instructions.push_back(new StackPushByte(0x00));
    }

    else if (token->name == "random-linear")
    {
        currentScope->block->instructions.push_back(new StackPushByte(0x01));
    }

    else
    {
        double base = 0;

        switch (token->name[0])
        {
            case 'c':
                break;

            case 'd':
                base = 2;

                break;

            case 'e':
                base = 4;

                break;

            case 'f':
                base = 5;

                break;

            case 'g':
                base = 7;

                break;

            case 'a':
                base = 9;

                break;

            case 'b':
                base = 11;

                break;

            default:
                return Utils::parseError("Variable \"" + token->name + "\" not defined.", sourcePath, token->line, token->character);
        }

        if (token->name.size() == 2 && isdigit(token->name[1]))
        {
            currentScope->block->instructions.push_back(new StackPushDouble(getFrequency(base + 12 * (token->name[1] - 48))));

            return;
        }

        else if (token->name.size() == 3 && isdigit(token->name[2]))
        {
            if (token->name[1] == 's')
            {
                currentScope->block->instructions.push_back(new StackPushDouble(getFrequency(base + 12 * (token->name[2] - 48) + 1)));

                return;
            }

            else if (token->name[1] == 'f')
            {
                currentScope->block->instructions.push_back(new StackPushDouble(getFrequency(base + 12 * (token->name[2] - 48) - 1)));

                return;
            }
        }

        return Utils::parseError("Unrecognized symbol \"" + token->name + "\".", sourcePath, token->line, token->character);
    }
}

void BytecodeTransformer::visit(const Constant* token)
{
    currentScope->block->instructions.push_back(new StackPushDouble(token->value));
}

void BytecodeTransformer::visit(const ListToken* token)
{
    for (int i = token->values.size() - 1; i >= 0; i--)
    {
        token->values[i]->accept(this);
    }

    currentScope->block->instructions.push_back(new StackPushInt(token->values.size()));
    currentScope->block->instructions.push_back(new CallNative("list"));
}

void BytecodeTransformer::visit(const Add* token)
{
    token->value2->accept(this);
    token->value1->accept(this);

    // used to constant fold here but it is now impossible
    // maybe do earlier in parsing or have separate visitor(s) for optimizations

    currentScope->block->instructions.push_back(new CallNative("add"));
}

void BytecodeTransformer::visit(const Subtract* token)
{
    token->value2->accept(this);
    token->value1->accept(this);

    // used to constant fold here but it is now impossible
    // maybe do earlier in parsing or have separate visitor(s) for optimizations

    currentScope->block->instructions.push_back(new CallNative("subtract"));
}

void BytecodeTransformer::visit(const Multiply* token)
{
    token->value2->accept(this);
    token->value1->accept(this);

    // used to constant fold here but it is now impossible
    // maybe do earlier in parsing or have separate visitor(s) for optimizations

    currentScope->block->instructions.push_back(new CallNative("multiply"));
}

void BytecodeTransformer::visit(const Divide* token)
{
    token->value2->accept(this);
    token->value1->accept(this);

    // used to constant fold here but it is now impossible
    // maybe do earlier in parsing or have separate visitor(s) for optimizations

    currentScope->block->instructions.push_back(new CallNative("divide"));
}

void BytecodeTransformer::visit(const Assign* token)
{
    currentVariable = token->variable->name;

    currentScope->addVariable(currentVariable);

    token->value->accept(this);

    currentScope->block->instructions.push_back(new SetVariable(currentVariable));

    currentVariable = "";
}

void BytecodeTransformer::visit(const Call* token)
{
    if (token->name->value)
    {
        return Utils::parseError("\"#\" can only precede variable names.", sourcePath, token->name->line, token->name->character);
    }

    const std::string name = token->name->name;

    if (name == "sine")
    {
        token->arguments->get("frequency", new Constant(0, 0, "0"), this);
        token->arguments->get("effects", new ListToken(0, 0, std::vector<Token*>()), this);
        token->arguments->get("pan", new Constant(0, 0, "0"), this);
        token->arguments->get("volume", new Constant(0, 0, "0"), this);
    }

    else if (name == "square")
    {
        token->arguments->get("frequency", new Constant(0, 0, "0"), this);
        token->arguments->get("effects", new ListToken(0, 0, std::vector<Token*>()), this);
        token->arguments->get("pan", new Constant(0, 0, "0"), this);
        token->arguments->get("volume", new Constant(0, 0, "0"), this);
    }

    else if (name == "saw")
    {
        token->arguments->get("frequency", new Constant(0, 0, "0"), this);
        token->arguments->get("effects", new ListToken(0, 0, std::vector<Token*>()), this);
        token->arguments->get("pan", new Constant(0, 0, "0"), this);
        token->arguments->get("volume", new Constant(0, 0, "0"), this);
    }

    else if (name == "triangle")
    {
        token->arguments->get("frequency", new Constant(0, 0, "0"), this);
        token->arguments->get("effects", new ListToken(0, 0, std::vector<Token*>()), this);
        token->arguments->get("pan", new Constant(0, 0, "0"), this);
        token->arguments->get("volume", new Constant(0, 0, "0"), this);
    }

    else if (name == "noise")
    {
        token->arguments->get("effects", new ListToken(0, 0, std::vector<Token*>()), this);
        token->arguments->get("pan", new Constant(0, 0, "0"), this);
        token->arguments->get("volume", new Constant(0, 0, "0"), this);
    }

    else if (name == "hold")
    {
        token->arguments->get("length", new Constant(0, 0, "0"), this);
        token->arguments->get("value", new Constant(0, 0, "0"), this);
    }

    else if (name == "lfo")
    {
        token->arguments->get("length", new Constant(0, 0, "0"), this);
        token->arguments->get("to", new Constant(0, 0, "0"), this);
        token->arguments->get("from", new Constant(0, 0, "0"), this);
    }

    else if (name == "sweep")
    {
        token->arguments->get("length", new Constant(0, 0, "0"), this);
        token->arguments->get("to", new Constant(0, 0, "0"), this);
        token->arguments->get("from", new Constant(0, 0, "0"), this);
    }

    else if (name == "sequence")
    {
        token->arguments->get("order", new Name(0, 0, "sequence-forwards"), this);
        token->arguments->get("values", new ListToken(0, 0, std::vector<Token*>()), this);
    }

    else if (name == "repeat")
    {
        token->arguments->get("repeats", new Constant(0, 0, "0"), this);
        token->arguments->get("value", new Constant(0, 0, "0"), this);
    }

    else if (name == "random")
    {
        token->arguments->get("type", new Name(0, 0, "random-step"), this);
        token->arguments->get("length", new Constant(0, 0, "0"), this);
        token->arguments->get("to", new Constant(0, 0, "0"), this);
        token->arguments->get("from", new Constant(0, 0, "0"), this);
    }

    else if (name == "limit")
    {
        token->arguments->get("max", new Constant(0, 0, "0"), this);
        token->arguments->get("min", new Constant(0, 0, "0"), this);
        token->arguments->get("value", new Constant(0, 0, "0"), this);
    }

    else if (name == "delay")
    {
        token->arguments->get("feedback", new Constant(0, 0, "0"), this);
        token->arguments->get("delay", new Constant(0, 0, "0"), this);
        token->arguments->get("mix", new Constant(0, 0, "0"), this);
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
        return Utils::parseError("Unknown function name \"" + name + "\".", sourcePath, token->line, token->character);
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

double BytecodeTransformer::getFrequency(const double note) const
{
    return 440 * pow(2, (note - 57) / 12);
}
