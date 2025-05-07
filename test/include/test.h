#pragma once

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_set>

#include "location.h"
#include "parse.h"
#include "path.h"
#include "token.h"
#include "tokenize.h"

struct TestInfo
{
    TestInfo(const Path* path);

    std::vector<std::vector<std::string>> data;
};

struct Test
{
    virtual unsigned int test() = 0;

protected:
    const Path* sourcePath(const std::string file) const;

    Parser::Program* parseSource(const Path* path) const;

    void print(const std::string text);

    void beginSection(const std::string name);
    void endSection();

    void beginTest(const std::string name);
    void endTest();

    void assert(const std::string name, const bool result);
    void fail(const std::string message);

    unsigned int indents = 0;

    unsigned int suiteFailures;
    unsigned int sectionFailures;
    unsigned int testFailures;

};

struct TokenizerTests : public Test
{
    unsigned int test() override;

private:
    void checkList(const Path* path);

};

struct ParserTests : public Test
{
    unsigned int test() override;

private:
    void roundTrip(const Path* path);
    void expectError(const Path* path);

    std::string formatSource(const std::string text) const;

};

struct TypeResolverTests : public Test
{
    unsigned int test() override;

private:
    void expectSuccess(const Path* path);
    void expectError(const Path* path);

};
