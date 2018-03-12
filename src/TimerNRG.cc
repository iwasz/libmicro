/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Hal.h"
#include "Timer.h"
#include <cstdint>

static volatile uint32_t sysTickCount;

/*****************************************************************************/

void HAL_IncTick () { sysTickCount++; }

/*****************************************************************************/

void HAL_InitTick ()
{
        /** Configure SysTick to generate Interrupt with 1ms period */
        SysTick_Config (SYSCLK_FREQ / 1000 - 1);
}

/*****************************************************************************/

uint32_t HAL_GetTick () { return sysTickCount; }