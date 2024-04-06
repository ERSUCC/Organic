#pragma once

#include <fstream>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "audiosource.h"
#include "controller.h"
#include "object.h"
#include "utils.h"

struct Machine
{
    Machine(const std::string path);

    void execute(unsigned int address);

private:
    unsigned int readInt(const unsigned int address) const;
    double readDouble(const unsigned int address) const;

    Object* popStack();
    template <typename T> T* popStackAs();
    template <typename T> List<T>* popStackAsList();

    Utils* utils;

    const std::string path;

    std::string program;

    std::stack<Object*> stack;

    std::unordered_map<unsigned char, Variable*> variables;

};
