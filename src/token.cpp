#include "../include/token.h"

Token::Token(const int line, const int character, const std::string str) :
    line(line), character(character), str(str) {}

std::string Token::string() const
{
    return str;
}

Token* Token::copy() const
{
    return new Token(line, character, str);
}

Object* Token::accept(ProgramVisitor* visitor) const
{
    return nullptr;
}

TokenRange::TokenRange(const int start, const int end, Token* token) :
    start(start), end(end), token(token) {}

OpenParenthesis::OpenParenthesis(const int line, const int character) :
    Token(line, character, "(") {}

Token* OpenParenthesis::copy() const
{
    return new OpenParenthesis(line, character);
}

CloseParenthesis::CloseParenthesis(const int line, const int character) :
    Token(line, character, ")") {}

Token* CloseParenthesis::copy() const
{
    return new CloseParenthesis(line, character);
}

Colon::Colon(const int line, const int character) :
    Token(line, character, ":") {}

Token* Colon::copy() const
{
    return new Colon(line, character);
}

Comma::Comma(const int line, const int character) :
    Token(line, character, ",") {}

Token* Comma::copy() const
{
    return new Comma(line, character);
}

Equals::Equals(const int line, const int character) :
    Token(line, character, "=") {}

Token* Equals::copy() const
{
    return new Equals(line, character);
}

Operator::Operator(const int line, const int character, const std::string str) :
    Token(line, character, str) {}

AddToken::AddToken(const int line, const int character) :
    Operator(line, character, "+") {}

Token* AddToken::copy() const
{
    return new AddToken(line, character);
}

SubtractToken::SubtractToken(const int line, const int character) :
    Operator(line, character, "-") {}

Token* SubtractToken::copy() const
{
    return new SubtractToken(line, character);
}

MultiplyToken::MultiplyToken(const int line, const int character) :
    Operator(line, character, "*") {}

Token* MultiplyToken::copy() const
{
    return new MultiplyToken(line, character);
}

DivideToken::DivideToken(const int line, const int character) :
    Operator(line, character, "/") {}

Token* DivideToken::copy() const
{
    return new DivideToken(line, character);
}

Name::Name(const int line, const int character, const std::string name, const bool value) :
    Token(line, character, name), name(name), value(value) {}

Token* Name::copy() const
{
    return new Name(line, character, name);
}

Object* Name::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
}

Constant::Constant(const int line, const int character, const std::string str) :
    Token(line, character, str), value(std::stod(str)) {}

Token* Constant::copy() const
{
    return new Constant(line, character, str);
}

Object* Constant::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
}

Argument::Argument(const int line, const int character, const std::string name, const Token* value) :
    Token(line, character, name + ": " + value->string()), name(name), value(value) {}

Token* Argument::copy() const
{
    return new Argument(line, character, name, value);
}

List::List(const int line, const int character, const std::vector<Token*> values) :
    Token(line, character), values(values) {}

std::string List::string() const
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

Token* List::copy() const
{
    return new List(line, character, values);
}

Combine::Combine(const Token* value1, const Token* value2, const std::string op) :
    Token(value1->line, value1->character, value1->string() + " " + op + " " + value2->string()), value1(value1), value2(value2) {}

Add::Add(const Token* value1, const Token* value2) :
    Combine(value1, value2, "+") {}

Token* Add::copy() const
{
    return new Add(value1, value2);
}

Object* Add::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
}

Subtract::Subtract(const Token* value1, const Token* value2) :
    Combine(value1, value2, "-") {}

Token* Subtract::copy() const
{
    return new Subtract(value1, value2);
}

Object* Subtract::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
}

Multiply::Multiply(const Token* value1, const Token* value2) :
    Combine(value1, value2, "*") {}

Token* Multiply::copy() const
{
    return new Multiply(value1, value2);
}

Object* Multiply::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
}

Divide::Divide(const Token* value1, const Token* value2) :
    Combine(value1, value2, "/") {}

Token* Divide::copy() const
{
    return new Divide(value1, value2);
}

Object* Divide::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
}

Instruction::Instruction(const int line, const int character, const std::string str) :
    Token(line, character, str) {}

Assign::Assign(const Name* variable, const Token* value) :
    Instruction(variable->line, variable->character, variable->string() + " = " + value->string()), variable(variable), value(value) {}

Token* Assign::copy() const
{
    return new Assign(variable, value);
}

Object* Assign::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
}

Call::Call(const Name* name, const std::vector<Argument*> arguments) :
    Instruction(name->line, name->character), name(name), arguments(arguments) {}

std::string Call::string() const
{
    std::string result = name->name + "(";

    if (arguments.size() == 0)
    {
        return result + ")";
    }

    result += arguments[0]->string();

    for (int i = 1; i < arguments.size(); i++)
    {
        result += ", " + arguments[i]->string();
    }

    return result + ")";
}

Token* Call::copy() const
{
    return new Call(name, arguments);
}

Object* Call::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
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

Token* Program::copy() const
{
    return new Program(instructions);
}

Object* Program::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
}

ProgramVisitor::ProgramVisitor(const std::string path) : path(path) {}

Object* ProgramVisitor::visit(const Name* token)
{
    if (variables.count(token->name))
    {
        if (currentVariable == token->name)
        {
            Utils::parseError("Variable \"" + token->name + "\" referenced in its own definition.", path, token->line, token->character);
        }

        variablesUsed.insert(token->name);

        if (token->value)
        {
            return variables[token->name]->value;
        }

        return variables[token->name];
    }

    if (token->value)
    {
        Utils::parseError("\"#\" can only precede variable names.", path, token->line, token->character);
    }

    if (token->name == "sequence-forwards")
    {
        return new Sequence::Order(Sequence::OrderEnum::Forwards);
    }

    if (token->name == "sequence-backwards")
    {
        return new Sequence::Order(Sequence::OrderEnum::Backwards);
    }

    if (token->name == "sequence-ping-pong")
    {
        return new Sequence::Order(Sequence::OrderEnum::PingPong);
    }

    if (token->name == "sequence-random")
    {
        return new Sequence::Order(Sequence::OrderEnum::Random);
    }

    if (token->name == "random-step")
    {
        return new Random::Type(Random::TypeEnum::Step);
    }

    if (token->name == "random-linear")
    {
       return new Random::Type(Random::TypeEnum::Linear);
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
            {
                Utils::parseError("Variable \"" + token->name + "\" not defined.", path, token->line, token->character);

                return nullptr;
            }
        }

        if (token->name.size() == 2 && isdigit(token->name[1]))
        {
            return new Value(getFrequency(base + 12 * (token->name[1] - 48)));
        }

        if (token->name.size() == 3 && isdigit(token->name[2]))
        {
            if (token->name[1] == 's')
            {
                return new Value(getFrequency(base + 12 * (token->name[2] - 48) + 1));
            }

            if (token->name[1] == 'f')
            {
                return new Value(getFrequency(base + 12 * (token->name[2] - 48) - 1));
            }
        }
    }

    Utils::parseError("Unrecognized symbol \"" + token->name + "\".", path, token->line, token->character);

    return nullptr;
}

Object* ProgramVisitor::visit(const Constant* token)
{
    return new Value(token->value);
}

Object* ProgramVisitor::visit(const Add* token)
{
    ValueObject* value1 = (ValueObject*)token->value1->accept(this);
    ValueObject* value2 = (ValueObject*)token->value2->accept(this);

    if (dynamic_cast<Value*>(value1) && dynamic_cast<Value*>(value2))
    {
        return new Value(value1->getValue() + value2->getValue());
    }

    return new ValueAdd(value1, value2);
}

Object* ProgramVisitor::visit(const Subtract* token)
{
    ValueObject* value1 = (ValueObject*)token->value1->accept(this);
    ValueObject* value2 = (ValueObject*)token->value2->accept(this);

    if (dynamic_cast<Value*>(value1) && dynamic_cast<Value*>(value2))
    {
        return new Value(value1->getValue() - value2->getValue());
    }

    return new ValueSubtract(value1, value2);
}

Object* ProgramVisitor::visit(const Multiply* token)
{
    ValueObject* value1 = (ValueObject*)token->value1->accept(this);
    ValueObject* value2 = (ValueObject*)token->value2->accept(this);

    if (dynamic_cast<Value*>(value1) && dynamic_cast<Value*>(value2))
    {
        return new Value(value1->getValue() * value2->getValue());
    }

    return new ValueMultiply(value1, value2);
}

Object* ProgramVisitor::visit(const Divide* token)
{
    ValueObject* value1 = (ValueObject*)token->value1->accept(this);
    ValueObject* value2 = (ValueObject*)token->value2->accept(this);

    if (dynamic_cast<Value*>(value1) && dynamic_cast<Value*>(value2))
    {
        return new Value(value1->getValue() / value2->getValue());
    }

    return new ValueDivide(value1, value2);
}

Object* ProgramVisitor::visit(const Assign* token)
{
    if (!variables.count(token->variable->name))
    {
        variables.insert(std::make_pair(token->variable->name, new Variable()));
    }

    currentVariable = token->variable->name;

    variables[token->variable->name]->value = (ValueObject*)token->value->accept(this);

    currentVariable = "";

    return variables[token->variable->name];
}

Object* ProgramVisitor::visit(const Call* token)
{
    if (token->name->value)
    {
        Utils::parseError("\"#\" can only precede variable names.", path, token->name->line, token->name->character);
    }

    std::unordered_map<std::string, const Argument*> arguments;

    for (const Argument* argument : token->arguments)
    {
        if (arguments.count(argument->name))
        {
            Utils::parseError("Input \"" + argument->name + "\" specified more than once.", path, argument->line, argument->character);
        }

        arguments[argument->name] = argument;
    }

    const std::string name = token->name->name;

    if (name == "sine" || name == "square" || name == "saw" || name == "triangle")
    {
        ValueObject* volume = new Value(0);
        ValueObject* pan = new Value(0);
        std::vector<Effect*> effects;
        ValueObject* frequency = new Value(0);

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "volume")
            {
                volume = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "pan")
            {
                pan = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "effects")
            {
                for (const Token* value : getList(argument.second->value))
                {
                    effects.push_back((Effect*)value->accept(this));
                }
            }

            else if (argument.first == "frequency")
            {
                frequency = (ValueObject*)argument.second->value->accept(this);
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        Oscillator* oscillator;

        if (name == "sine")
        {
            oscillator = new Sine(volume, pan, effects, frequency);
        }
        
        else if (name == "square")
        {
            oscillator = new Square(volume, pan, effects, frequency);
        }

        else if (name == "saw")
        {
            oscillator = new Saw(volume, pan, effects, frequency);
        }

        else if (name == "triangle")
        {
            oscillator = new Triangle(volume, pan, effects, frequency);
        }

        sources.push_back(oscillator);

        return oscillator;
    }

    else if (name == "noise")
    {
        ValueObject* volume = new Value(0);
        ValueObject* pan = new Value(0);
        std::vector<Effect*> effects;

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "volume")
            {
                volume = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "pan")
            {
                pan = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "effects")
            {
                for (const Token* value : getList(argument.second->value))
                {
                    effects.push_back((Effect*)value->accept(this));
                }
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        Noise* noise = new Noise(volume, pan, effects);

        sources.push_back(noise);

        return noise;
    }

    else if (name == "hold")
    {
        ValueObject* value = new Value(0);
        ValueObject* length = new Value(0);

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "value")
            {
                value = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "length")
            {
                length = (ValueObject*)argument.second->value->accept(this);
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        return new Hold(value, length);
    }

    else if (name == "lfo" || name == "sweep")
    {
        ValueObject* from = new Value(0);
        ValueObject* to = new Value(1);
        ValueObject* length = new Value(0);

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "from")
            {
                from = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "to")
            {
                to = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "length")
            {
                length = (ValueObject*)argument.second->value->accept(this);
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        if (name == "lfo")
        {
            return new LFO(from, to, length);
        }

        return new Sweep(from, to, length);
    }

    else if (name == "sequence")
    {
        std::vector<ValueObject*> values;
        Sequence::Order* order = new Sequence::Order(Sequence::OrderEnum::Forwards);

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "values")
            {
                for (const Token* token : getList(argument.second->value))
                {
                    values.push_back((ValueObject*)token->accept(this));
                }
            }

            else if (argument.first == "order")
            {
                order = (Sequence::Order*)argument.second->value->accept(this);
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        return new Sequence(values, order);
    }

    else if (name == "repeat")
    {
        ValueObject* value = new Value(0);
        ValueObject* repeats = new Value(0);

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "value")
            {
                value = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "repeats")
            {
                repeats = (ValueObject*)argument.second->value->accept(this);
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        return new Repeat(value, repeats);
    }

    else if (name == "random")
    {
        ValueObject* from = new Value(0);
        ValueObject* to = new Value(1);
        ValueObject* length = new Value(0);
        Random::Type* type = new Random::Type(Random::TypeEnum::Step);

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "from")
            {
                from = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "to")
            {
                to = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "length")
            {
                length = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "type")
            {
                type = (Random::Type*)argument.second->value->accept(this);
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        return new Random(from, to, length, type);
    }

    else if (name == "delay")
    {
        ValueObject* mix = new Value(1);
        ValueObject* delay = new Value(0);
        ValueObject* feedback = new Value(0);

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "mix")
            {
                mix = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "delay")
            {
                delay = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "feedback")
            {
                feedback = (ValueObject*)argument.second->value->accept(this);
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        return new Delay(mix, delay, feedback);
    }

    else if (name == "cutoff")
    {
        ValueObject* mix = new Value(1);
        ValueObject* cutoff = new Value(0);

        for (std::pair<const std::string, const Argument*> argument : arguments)
        {
            if (argument.first == "mix")
            {
                mix = (ValueObject*)argument.second->value->accept(this);
            }

            else if (argument.first == "cutoff")
            {
                cutoff = (ValueObject*)argument.second->value->accept(this);
            }

            else
            {
                Utils::parseError("Invalid input name \"" + argument.first + "\" for function \"" + name + "\".", path, argument.second->line, argument.second->character);
            }
        }

        return new LowPassFilter(mix, cutoff);
    }

    Utils::parseError("Unknown function name \"" + name + "\".", path, token->line, token->character);

    return nullptr;
}

Object* ProgramVisitor::visit(const Program* token)
{
    for (const Instruction* instruction : token->instructions)
    {
        instruction->accept(this);
    }

    for (std::pair<std::string, Variable*> pair : variables)
    {
        if (!variablesUsed.count(pair.first))
        {
            Utils::warning("Warning: Unused variable \"" + pair.first + "\".");
        }
    }

    return nullptr;
}

const std::vector<Token*> ProgramVisitor::getList(const Token* token) const
{
    if (dynamic_cast<const List*>(token))
    {
        return ((List*)token)->values;
    }

    return std::vector<Token*> { token->copy() };
}

double ProgramVisitor::getFrequency(const double note) const
{
    return 440 * pow(2, (note - 45) / 12);
}
