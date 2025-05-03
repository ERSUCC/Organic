#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#include <RtAudio.h>
#include <sndfile.hh>

#include "flags.h"
#include "machine.h"
#include "parse.h"
#include "path.h"
#include "token.h"
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

    Machine* machine;

    bool callbackActive = false;

};
