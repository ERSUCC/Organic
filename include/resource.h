#pragma once

#include <samplerate.h>
#include <sndfile.hh>

#include "exception.h"
#include "object.h"
#include "path.h"

namespace Engine {

struct Resource : public ValueObject
{
    Resource(const Path* path, const SourceLocation location);
    ~Resource();

    double* samples;

    unsigned int length;
};

}
