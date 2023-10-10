#include "../include/config.h"

unsigned int Config::SAMPLE_RATE = 44100;
unsigned int Config::CHANNELS = 2;
unsigned int Config::BUFFER_LENGTH = 128;
double Config::TWO_PI = M_PI * 2;
double Config::MASTER_VOLUME = 0.25;
double Config::TIME = 0;
std::mt19937 Config::RNG = std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count());