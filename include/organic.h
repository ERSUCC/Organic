#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <limits.h>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include <RtAudio.h>
#include <sndfile.hh>

#include "exception.h"
#include "flags.h"
#include "parse.h"
#include "path.h"
#include "program.h"
#include "resolve.h"
#include "token.h"
#include "transform.h"
#include "utils.h"

struct Organic
{
    Organic(const Path* path, const ProgramOptions options);

    void start();

private:
    void startPlayback();
    void startExport();

    int processAudio(void* output, unsigned int frames);

    void audioError(const std::string message) const;

    const ProgramOptions options;

    Utils* utils;

    Engine::Program* program;

};
