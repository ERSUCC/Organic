#pragma once

#include <samplerate.h>
#include <sndfile.hh>
#include <unordered_map>
#include <utility>

#include "exception.h"
#include "object.h"
#include "path.h"
#include "source.h"

namespace Engine {

struct Resource : public ValueObject
{
    Resource(const Path& path, const SourceLocation& location);
    Resource();
    ~Resource();

    double* samples;

    size_t length;
};

struct ResourceLocator : public ValueObject
{
    ResourceLocator(ValueObject* object);
    ~ResourceLocator();

    ValueObject* getLeaf() override;

    void loadResources(const SourceLocation& location);

protected:
    void init() override;
    void compute() override;

private:
    ValueObject* object;

    std::unordered_map<String*, Resource*> resources;

};

}
