#pragma once

#include <fstream>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>

#include "utils.h"
#include "object.h"

struct ObjectPool
{
    unsigned int allocate(Object* object);

    void deallocate(unsigned int address);

private:
    std::unordered_map<unsigned int, Object*> heap;
    std::set<unsigned int> free;

    unsigned int end;

};

struct Machine
{
    Machine(const std::string path);

    void execute(unsigned int address);

private:
    unsigned int addObject(Object* object);

    void removeObject(unsigned int address);

    Utils* utils;

    const std::string path;

    std::string program;

    ObjectPool* objectPool;

    std::stack<std::vector<unsigned char>> stack;

};
