#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

#include <RtAudio.h>
#include <sndfile.hh>

#include "exception.h"
#include "flags.h"
#include "object.h"
#include "parse.h"
#include "path.h"
#include "program.h"
#include "resolve.h"
#include "token.h"
#include "transform.h"
#include "utils.h"

struct Organic
{
    Organic(const ProgramOptions& options);
    ~Organic();

    void start();
    void stop();

private:
    void startPlayback();
    void startExport();

    int processAudio(void* output, const unsigned int frames);

    void audioError(const std::string& message) const;

    void handleInterrupt(Organic* organic);

    const ProgramOptions options;

    Utils* utils;

    Engine::Program* program;

    std::condition_variable signal;
    std::mutex lock;

    bool running = true;

};
