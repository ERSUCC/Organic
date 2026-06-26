#include "../include/program.h"

using namespace Engine;

Program::Program(const std::vector<AudioSource*>& audioSources) :
    audioSources(audioSources) {}

Program::~Program()
{
    for (AudioSource* audioSource : audioSources)
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
