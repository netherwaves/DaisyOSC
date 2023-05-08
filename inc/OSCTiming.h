#pragma once

#include "daisy_core.h"

typedef struct
{
    uint32_t seconds;
    uint32_t fractionofseconds;
} osctime_t;

osctime_t oscTime();
// int adcRead(int pin, osctime_t *t);
// int capacitanceRead(int pin, osctime_t *t);

// int inputRead(int pin, uint64_t *t);