#pragma once

#include <iostream>
#include <string>
#include <unordered_set>

#include "location.h"
#include "parse.h"
#include "path.h"
#include "token.h"

struct Test
{
    void testAll();

protected:
    const Path* sourcePath(const std::string file) const;
    const Parser::Program* parseSource(const Path* path) const;

    void print(const std::string text);

    void beginSection(const std::string name);
    void endSection();

    void beginTest(const std::string name);
    void endTest();

    void assert(const std::string name, const bool result);

    unsigned int indents = 0;
    unsigned int failures;

};

struct ParserTest : public Test
{

public:
    void test();

private:
    bool semanticEquals(const Parser::Token* expected, const Parser::Token* actual) const;

};
