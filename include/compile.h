#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "parse.h"
#include "token.h"

struct CompilerResult
{
    std::vector<AudioSource*> sources;
    EventQueue* eventQueue;
};

struct Compiler
{
    static CompilerResult compile(char* path);
};