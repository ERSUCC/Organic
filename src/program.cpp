#include "../include/program.h"

using namespace Engine;

Program::Program(const std::vector<ValueObject*>& variables, const std::vector<AudioSource*>& audioSources) :
    variables(variables), audioSources(audioSources) {}

Program::~Program()
{
    for (const ValueObject* variable : variables)
    {
        delete variable;
    }

    for (const AudioSource* audioSource : audioSources)
    {
        delete audioSource;
    }
}

void Program::processAudioSources(double* buffer) const
{
    memset(buffer, 0, sizeof(double) * utils->channels);

    for (AudioSource* audioSource : audioSources)
    {
        audioSource->fillBuffer(buffer);
    }
}

void Program::init()
{
    for (AudioSource* audioSource : audioSources)
    {
        audioSource->start(startTime);
    }
}
