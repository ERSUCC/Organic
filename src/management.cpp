#include "../include/management.h"

void AudioSourceManager::addAudioSource(AudioSourceTrait* audioSource)
{
    audioSources.insert(audioSource);
}

void AudioSourceManager::removeAudioSource(AudioSourceTrait* audioSource)
{
    audioSources.erase(audioSource);
}

void AudioSourceManager::fillBuffer(double* buffer, const unsigned int bufferLength, const int channels, const double masterVolume)
{
    std::fill(buffer, buffer + bufferLength * channels, 0);

    for (AudioSourceTrait* audioSource : audioSources)
    {
        audioSource->fillBuffer(buffer, bufferLength, masterVolume);
    }
}

void EventManager::addEvent(EventTrait* event)
{
    events.push_back(event);
}

void EventManager::performEvents()
{
    for (int i = 0; i < events.size(); i++)
    {
        if (events[i]->ready())
        {
            events[i]->perform();

            if (!events[i]->getNext())
            {
                events.erase(events.begin() + i);

                i--;
            }
        }
    }
}
