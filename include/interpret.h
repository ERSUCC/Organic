#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "parse.h"
#include "token.h"

struct InterpreterResult
{
    std::vector<AudioSource*> sources;
    EventQueue* eventQueue;
};

struct Interpreter
{
    static InterpreterResult interpret(char* path);
};