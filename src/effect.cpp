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

Delay::Delay(double delay, double feedback) : delay(delay), feedback(feedback) {}

void Delay::apply(double time)
{
    for (AudioSource* audioSource : connectedSources)
    {
        for (int i = 0; i < Config::BUFFER_LENGTH * Config::CHANNELS; i += Config::CHANNELS)
        {
            if (Config::CHANNELS == 1)
            {
                if (time >= bufferTime + delay.value)
                {
                    audioSource->effectBuffer[i] += buffer.front() * feedback.value;

                    buffer.pop();

                    bufferTime = time - delay.value;
                }

                buffer.push(audioSource->effectBuffer[i]);
            }

            else
            {
                if (time >= bufferTime + delay.value)
                {
                    audioSource->effectBuffer[i] += buffer.front() * feedback.value;

                    buffer.pop();

                    audioSource->effectBuffer[i + 1] += buffer.front() * feedback.value;

                    buffer.pop();

                    bufferTime = time - delay.value;
                }

                buffer.push(audioSource->effectBuffer[i]);
                buffer.push(audioSource->effectBuffer[i + 1]);
            }
        }
    }
}