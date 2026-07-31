#include "../include/program.h"

using namespace Engine;

Program::Program(const std::vector<ValueObject*>& variables, const std::vector<ValueObject*>& audioSources) :
    variables(variables), audioSources(audioSources) {}

Program::~Program()
{
    for (const ValueObject* variable : variables)
    {
        delete variable;
    }

    for (const ValueObject* audioSource : audioSources)
    {
        delete audioSource;
    }
}

void Program::processAudioSources(double* buffer) const
{
    memset(buffer, 0, sizeof(double) * utils->channels);

    for (ValueObject* audioSource : audioSources)
    {
        audioSource->update();
        audioSource->getLeafAs<AudioSource>()->fillBuffer(buffer);
    }
}

void Program::init()
{
    for (ValueObject* audioSource : audioSources)
    {
        audioSource->start(startTime);
    }
}
