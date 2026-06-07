#pragma once

#include <samplerate.h>
#include <sndfile.hh>

#include "object.h"
#include "path.h"
#include "utils.h"

namespace Engine
{

struct Resource : public ValueObject
{
    Resource(const Path* path, const SourceLocation location);
    ~Resource();

    double* samples;

    unsigned int length;
};

}
