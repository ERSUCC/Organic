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

ArgumentList::ArgumentList(const std::vector<Argument*> arguments, const std::string name, const std::string path) :
    argumentsOrdered(arguments), name(name), path(path)
{
    for (const Argument* argument : arguments)
    {
        if (this->arguments.count(argument->name))
        {
            Utils::parseError("Input \"" + argument->name + "\" specified more than once.", path, argument->line, argument->character);
        }

        this->arguments[argument->name] = argument;
    }
}

Object* ArgumentList::get(const std::string name, Object* defaultValue, ProgramVisitor* visitor)
{
    if (arguments.count(name))
    {
        Object* value = arguments[name]->value->accept(visitor);

        arguments.erase(name);

        int i = 0;

        while (i < argumentsOrdered.size() && argumentsOrdered[i]->name != name)
        {
            i++;
        }

        argumentsOrdered.erase(argumentsOrdered.begin() + i);

        return value;
    }

    return defaultValue;
}

void ArgumentList::confirmEmpty() const
{
    if (!arguments.empty())
    {
        Argument* argument = argumentsOrdered[0];

        Utils::parseError("Invalid input name \"" + argument->name + "\" for function \"" + name + "\".", path, argument->line, argument->character);
    }
}

std::string ArgumentList::string() const
{
    if (argumentsOrdered.empty())
    {
        return "";
    }

    std::string result = argumentsOrdered[0]->string();

    for (int i = 1; i < argumentsOrdered.size(); i++)
    {
        result += ", " + argumentsOrdered[i]->string();
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

Token* ListToken::copy() const
{
    return new ListToken(line, character, values);
}

Object* ListToken::accept(ProgramVisitor* visitor) const
{
    return visitor->visit(this);
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

Call::Call(const Name* name, ArgumentList* arguments) :
    Instruction(name->line, name->character), name(name), arguments(arguments) {}

std::string Call::string() const
{
    return name->name + "(" + arguments->string() + ")";
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

Object* ProgramVisitor::visit(const ListToken* token)
{
    std::vector<Object*> objects;

    for (Token* token : token->values)
    {
        objects.push_back(token->accept(this));
    }

    return new List<Object>(objects);
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

    const std::string name = token->name->name;

    Object* object = nullptr;

    if (name == "sine")
    {
        Sine* sine = new Sine((ValueObject*)token->arguments->get("volume", new Value(0), this),
                              (ValueObject*)token->arguments->get("pan", new Value(0), this),
                              getList<Effect>(token->arguments->get("effects", new List<Effect>(std::vector<Effect*>()), this)),
                              (ValueObject*)token->arguments->get("frequency", new Value(0), this));

        sources.push_back(sine);

        object = sine;
    }

    else if (name == "square")
    {
        Square* square = new Square((ValueObject*)token->arguments->get("volume", new Value(0), this),
                                    (ValueObject*)token->arguments->get("pan", new Value(0), this),
                                    getList<Effect>(token->arguments->get("effects", new List<Effect>(std::vector<Effect*>()), this)),
                                    (ValueObject*)token->arguments->get("frequency", new Value(0), this));

        sources.push_back(square);

        object = square;
    }

    else if (name == "saw")
    {
        Saw* saw = new Saw((ValueObject*)token->arguments->get("volume", new Value(0), this),
                           (ValueObject*)token->arguments->get("pan", new Value(0), this),
                           getList<Effect>(token->arguments->get("effects", new List<Effect>(std::vector<Effect*>()), this)),
                           (ValueObject*)token->arguments->get("frequency", new Value(0), this));

        sources.push_back(saw);

        object = saw;
    }

    else if (name == "triangle")
    {
        Triangle* triangle = new Triangle((ValueObject*)token->arguments->get("volume", new Value(0), this),
                                          (ValueObject*)token->arguments->get("pan", new Value(0), this),
                                          getList<Effect>(token->arguments->get("effects", new List<Effect>(std::vector<Effect*>()), this)),
                                          (ValueObject*)token->arguments->get("frequency", new Value(0), this));

        sources.push_back(triangle);

        object = triangle;
    }

    else if (name == "noise")
    {
        Noise* noise = new Noise((ValueObject*)token->arguments->get("volume", new Value(0), this),
                                 (ValueObject*)token->arguments->get("pan", new Value(0), this),
                                 getList<Effect>(token->arguments->get("effects", new List<Effect>(std::vector<Effect*>()), this)));

        sources.push_back(noise);

        object = noise;
    }

    else if (name == "hold")
    {
        object = new Hold((ValueObject*)token->arguments->get("value", new Value(0), this),
                          (ValueObject*)token->arguments->get("length", new Value(0), this));
    }

    else if (name == "lfo")
    {
        object = new LFO((ValueObject*)token->arguments->get("from", new Value(0), this),
                         (ValueObject*)token->arguments->get("to", new Value(0), this),
                         (ValueObject*)token->arguments->get("length", new Value(0), this));
    }

    else if (name == "sweep")
    {
        object = new Sweep((ValueObject*)token->arguments->get("from", new Value(0), this),
                           (ValueObject*)token->arguments->get("to", new Value(0), this),
                           (ValueObject*)token->arguments->get("length", new Value(0), this));
    }

    else if (name == "sequence")
    {
        object = new Sequence(getList<ValueObject>(token->arguments->get("values", new List<ValueObject>(std::vector<ValueObject*>()), this)),
                              (Sequence::Order*)token->arguments->get("order", new Sequence::Order(Sequence::OrderEnum::Forwards), this));
    }

    else if (name == "repeat")
    {
        object = new Repeat((ValueObject*)token->arguments->get("value", new Value(0), this),
                            (ValueObject*)token->arguments->get("repeats", new Value(0), this));
    }

    else if (name == "random")
    {
        object = new Random((ValueObject*)token->arguments->get("from", new Value(0), this),
                            (ValueObject*)token->arguments->get("to", new Value(0), this),
                            (ValueObject*)token->arguments->get("length", new Value(0), this),
                            (Random::Type*)token->arguments->get("type", new Random::Type(Random::TypeEnum::Step), this));
    }

    else if (name == "delay")
    {
        object = new Delay((ValueObject*)token->arguments->get("mix", new Value(0), this),
                           (ValueObject*)token->arguments->get("delay", new Value(0), this),
                           (ValueObject*)token->arguments->get("feedback", new Value(0), this));
    }

    // low pass filter

    else
    {
        Utils::parseError("Unknown function name \"" + name + "\".", path, token->line, token->character);
    }

    token->arguments->confirmEmpty();

    return object;
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

template <typename T> List<T>* ProgramVisitor::getList(Object* object) const
{
    std::vector<T*> objects;

    if (List<Object>* list = dynamic_cast<List<Object>*>(object))
    {
        for (Object* object : list->objects)
        {
            objects.push_back(dynamic_cast<T*>(object));
        }
    }

    else
    {
        objects.push_back(dynamic_cast<T*>(object));
    }

    return new List<T>(objects);
}

double ProgramVisitor::getFrequency(const double note) const
{
    return 440 * pow(2, (note - 45) / 12);
}
