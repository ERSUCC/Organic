#include "../include/effect.h"

void EffectManager::addEffect(Effect* effect)
{
    effects.push_back(effect);
}

void EffectManager::removeEffect(Effect* effect)
{
    effects.erase(std::find(effects.begin(), effects.end(), effect));
}

void EffectManager::connectAudioSource(Effect* effect, AudioSource* audioSource)
{
    effect->connectedSources.push_back(audioSource);
}

void EffectManager::disconnectAudioSource(Effect* effect, AudioSource* audioSource)
{
    effect->connectedSources.erase(std::find(effect->connectedSources.begin(), effect->connectedSources.end(), audioSource));
}

void EffectManager::applyEffects(double time)
{
    for (Effect* effect : effects)
    {
        effect->apply(time);
    }
}

LowPassFilter::LowPassFilter(double cutoff) : cutoff(cutoff) {}

void LowPassFilter::apply(double time)
{
    for (AudioSource* audioSource : connectedSources)
    {
        for (int i = 0; i < Config::BUFFER_LENGTH * Config::CHANNELS; i += Config::CHANNELS)
        {
            if (Config::CHANNELS == 1)
            {
                
            }

            else
            {
                
            }
        }
    }
}