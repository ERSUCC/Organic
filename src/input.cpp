#include "input.h"

using namespace Engine;

InputManager::InputManager() :
    utils(Utils::get()) {}

InputManager::~InputManager()
{
    free(input);
}

void InputManager::activate()
{
    if (!activated)
    {
        input = (double*)malloc(sizeof(double) * utils->channels * utils->bufferLength);

        activated = true;
    }
}

void InputManager::processAudio(double* input)
{
    for (size_t i = 0; i < utils->channels; i++)
    {
        this->input[i] = input[i];
    }
}

void InputManager::readInput(double* buffer)
{
    for (size_t i = 0; i < utils->channels; i++)
    {
        buffer[i] = input[i];
    }
}
