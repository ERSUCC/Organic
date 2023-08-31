#pragma once

#include <vector>
#include <queue>

#include "audiosource.h"
#include "parameter.h"

struct EffectManager;

struct Effect
{
    friend struct EffectManager;

    virtual void apply(double time) = 0;

protected:
    std::vector<AudioSource*> connectedSources;

};

struct EffectManager
{
    void addEffect(Effect* effect);
    void removeEffect(Effect* effect);

    void connectAudioSource(Effect* effect, AudioSource* audioSource);
    void disconnectAudioSource(Effect* effect, AudioSource* audioSource);

    void applyEffects(double time);

private:
    std::vector<Effect*> effects;

};

struct Delay : public Effect
{
    Delay(double delay, double feedback);

    void apply(double time) override;

    Parameter delay;
    Parameter feedback;

private:
    std::queue<double> buffer;

    double bufferTime = 0;

};