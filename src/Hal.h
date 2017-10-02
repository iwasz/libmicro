/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef MICROLIB_HAL_H
#define MICROLIB_HAL_H

#if defined(LIB_MICRO_STM32F0)
#include "HalStm32F0.h"
#elif defined(LIB_MICRO_STM32F4)
#include "HalStm32F4.h"
#endif

#endif // HAL_H
