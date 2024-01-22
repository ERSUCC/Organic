#pragma once

#include <unordered_set>
#include <vector>

struct AudioSourceTrait
{
    virtual void fillBuffer(double* buffer, const unsigned int bufferLength, const double masterVolume) = 0;
};

struct AudioSourceManager
{
    void addAudioSource(AudioSourceTrait* audioSource);
    void removeAudioSource(AudioSourceTrait* audioSource);

    void fillBuffer(double* buffer, const unsigned int bufferLength, const int channels, const double masterVolume);

private:
    std::unordered_set<AudioSourceTrait*> audioSources;

};

struct EventTrait
{
    virtual void perform() = 0;

    virtual bool ready() = 0;
    virtual bool getNext() = 0;
};

struct EventManager
{
    void addEvent(EventTrait* event);

    void performEvents();

private:
    std::vector<EventTrait*> events;

};
