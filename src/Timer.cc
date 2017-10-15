#include "Timer.h"
#include "Hal.h"

Timer::Timer () : startTime (0), intervalMs (0) {}

/*****************************************************************************/

void Timer::start (uint32_t interval)
{
        startTime = HAL_GetTick ();
        this->intervalMs = interval;
}

/*****************************************************************************/

bool Timer::isExpired () const { return HAL_GetTick () - startTime > intervalMs; }
