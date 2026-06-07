#pragma once

#include <vector>

#include "audiosource.h"
#include "object.h"

namespace Engine {

struct Program : public ValueObject
{
    Program(const std::vector<AudioSource*>& audioSources);

    void processAudioSources(double* buffer) const;

protected:
    void init() override;

private:
    const std::vector<AudioSource*> audioSources;

};

}
