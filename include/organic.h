#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <string>
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
    Organic(const Path* path, const std::vector<std::string>& flags);

    void start();

private:
    void startPlayback();
    void startExport();

    int processAudio(void* output, unsigned int frames);

    Utils* utils;

    ProgramOptions options;

    Machine* machine;

    bool callbackActive = false;

};
