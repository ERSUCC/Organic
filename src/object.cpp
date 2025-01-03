#include "../include/object.h"

Sync::Sync() :
    utils(Utils::get()) {}

void Sync::start(double time)
{
    if (!enabled)
    {
        startTime = time;
        repeatTime = time;

        enabled = true;

        init();
    }
}

void Sync::repeat(double time)
{
    repeatTime = time;

    reinit();
}

void Sync::stop()
{
    enabled = false;
}

double Sync::syncLength() const
{
    return 0;
}

void Sync::init() {}
void Sync::reinit() {}

double ValueObject::getValue()
{
    return 0;
}

ValueObject* ValueObject::expandVariable()
{
    return this;
}

List* ValueObject::getList()
{
    return nullptr;
}

Resource* ValueObject::getResource()
{
    return nullptr;
}

Default::Default() :
    list(new List({})), resource(new Resource({}, utils->sampleRate)) {}

Default* Default::get()
{
    static Default* instance;

    if (!instance)
    {
        instance = new Default();
    }

    return instance;
}

List* Default::getList()
{
    return list;
}

Resource* Default::getResource()
{
    return resource;
}

List::List(const std::vector<ValueObject*> objects) :
    objects(objects) {}

List* List::getList()
{
    return this;
}

Variable::Variable(ValueObject* value) :
    value(value) {}

double Variable::syncLength() const
{
    return value->syncLength();
}

double Variable::getValue()
{
    if (!value->enabled)
    {
        stop();
    }

    return value->getValue();
}

ValueObject* Variable::expandVariable()
{
    return value->expandVariable();
}

List* Variable::getList()
{
    return value->getList();
}

Resource* Variable::getResource()
{
    return value->getResource();
}

void Variable::init()
{
    value->start(startTime);
}

Resource::Resource(const std::vector<double>& samples, const unsigned int sampleRate) :
    samples(samples), sampleRate(sampleRate) {}

Resource* Resource::getResource()
{
    return this;
}

double Time::getValue()
{
    return utils->time;
}
