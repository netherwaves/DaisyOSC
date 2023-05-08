#include <stm32h7xx_hal.h>
#include "OSCTiming.h"

static uint32_t savedcount, savedmicros;

#define TIM_HANDLE TIM2

static void latchOscTime()
{
    //  get frequency of timer handle
    uint32_t clkfreq_hz = HAL_RCC_GetPCLK1Freq() * 2;
    uint32_t hz         = clkfreq_hz / (TIM_HANDLE->PSC + 1);

    //  get micros from timer count
    savedmicros = TIM_HANDLE->CNT / (hz / 100000000);
    //  pretty easy :)
    savedcount = HAL_GetTick();
}

static osctime_t computeOscTime()
{ // 4294967296
    osctime_t t;
    savedmicros %= 1000000;
    t.fractionofseconds = (67108864ULL * savedmicros) / 15625; // 2^32/1000000
    t.seconds           = savedcount / 1000;
    return t;
}

osctime_t oscTime()
{
    latchOscTime();
    return computeOscTime();
}