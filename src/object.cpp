#include "object.h"

using namespace Engine;

void Defaults::deinit()
{
    for (const std::pair<std::type_index, ValueObject*>& pair : objects)
    {
        delete pair.second;
    }
}

ValueObject::ValueObject() :
    utils(Utils::get()) {}

ValueObject::~ValueObject() {}

ValueObject* ValueObject::clone() const
{
    return new ValueObject();
}

void ValueObject::start(double time)
{
    if (!enabled)
    {
        startTime = time;
        repeatTime = time;

        enabled = true;

        init();
    }
}

void ValueObject::repeat(double time)
{
    repeatTime = time;

    reinit();
}

void ValueObject::stop(const double time)
{
    if (enabled)
    {
        stopTime = time;
        enabled = false;
    }
}

void ValueObject::update()
{
    if (lastUpdate != utils->time)
    {
        lastUpdate = utils->time;

        compute();
    }
}

double ValueObject::getValue() const
{
    return 0;
}

ValueObject* ValueObject::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return this;
}

void ValueObject::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    leaves.insert(this);
}

void ValueObject::init() {}
void ValueObject::reinit() {}
void ValueObject::compute() {}

String::String(const std::string& value) :
    value(value) {}

String::String() :
    value("") {}


ValueObject* String::clone() const
{
    return new String(value);
}

List::List(const std::vector<ValueObject*>& objects) :
    objects(objects) {}

List::~List()
{
    for (const ValueObject* object : objects)
    {
        delete object;
    }
}

ValueObject* List::clone() const
{
    std::vector<ValueObject*> cloned;

    for (const ValueObject* object : objects)
    {
        cloned.push_back(object->clone());
    }

    return new List(cloned);
}

void List::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    for (ValueObject* object : objects)
    {
        object->getLeaves(leaves);
    }
}

MultiList::MultiList(ValueObject* object) :
    object(object) {}

MultiList::~MultiList()
{
    delete object;
}

ValueObject* MultiList::clone() const
{
    return new MultiList(object->clone());
}

ValueObject* MultiList::getLeaf()
{
    return object->getLeaf();
}

void MultiList::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    object->getLeaves(leaves);
}

void MultiList::init()
{
    object->start(startTime);

    currentList = object->getLeafAs<List>();

    for (ValueObject* object : currentList->objects)
    {
        object->start(startTime);
    }
}

void MultiList::compute()
{
    object->update();

    List* list = object->getLeafAs<List>();

    if (list != currentList)
    {
        for (ValueObject* object : list->objects)
        {
            object->start(list->getStartTime());
        }

        currentList = list;
    }

    for (ValueObject* object : currentList->objects)
    {
        object->update();
    }
}

Variable::Variable(ValueObject* value, const bool unique) :
    value(value), unique(unique) {}

ValueObject* Variable::clone() const
{
    if (unique)
    {
        return value->clone();
    }

    return new Variable(value, false);
}

double Variable::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return value->getValue();
}

ValueObject* Variable::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return value->getLeaf();
}

void Variable::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    value->getLeaves(leaves);
}

void Variable::init()
{
    value->start(startTime);
}

void Variable::compute()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
}

Fillable::Fillable(InputMap* inputs, const std::string& input, ValueObject* value) :
    inputs(inputs), input(input), value(value) {}

ValueObject* Fillable::clone() const
{
    return inputs->cloneInput(input, value);
}

double Fillable::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return value->getValue();
}

ValueObject* Fillable::getLeaf()
{
    if (!enabled)
    {
        return nullptr;
    }

    return value->getLeaf();
}

void Fillable::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    value->getLeaves(leaves);
}

void Fillable::init()
{
    value->start(startTime);
}

void Fillable::compute()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
}

InputMap::InputMap(const std::unordered_map<std::string, Fillable*>& inputs) :
    inputs(inputs) {}

InputMap* InputMap::clone()
{
    InputMap* map = new InputMap(cloned);

    cloned.clear();

    return map;
}

Fillable* InputMap::addInput(const std::string& input)
{
    Fillable* fillable = new Fillable(this, input, new ValueObject());

    inputs[input] = fillable;

    return fillable;
}

Fillable* InputMap::cloneInput(const std::string& input, ValueObject* value)
{
    Fillable* fillable = new Fillable(this, input, value);

    cloned[input] = fillable;

    return fillable;
}

void InputMap::setInput(const std::string& input, ValueObject* value)
{
    inputs.at(input)->value = value;
}

Lambda::Lambda(InputMap* inputs, ValueObject* value) :
    inputs(inputs), value(value) {}

Lambda::~Lambda()
{
    delete inputs;
    delete value;
}

ValueObject* Lambda::clone() const
{
    ValueObject* cloned = value->clone();

    return new Lambda(inputs->clone(), cloned);
}

double Lambda::getValue() const
{
    if (!enabled)
    {
        return 0;
    }

    return value->getValue();
}

ValueObject* Lambda::getLeaf()
{
    return value->getLeaf();
}

void Lambda::getLeaves(std::unordered_set<ValueObject*>& leaves)
{
    value->getLeaves(leaves);
}

void Lambda::setInput(const std::string& name, ValueObject* value)
{
    inputs->setInput(name, value);
}

void Lambda::init()
{
    value->start(startTime);
}

void Lambda::compute()
{
    value->update();

    if (!value->enabled)
    {
        stop(value->getStopTime());
    }
}
