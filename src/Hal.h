/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef MICROLIB_HAL_H
#define MICROLIB_HAL_H

#if defined(STM32F072xB) | defined (STM32F091xC)
#define LIB_MICRO_STM32F0
#define LIB_MICRO_STM32F
#define USE_USART1
#define USE_USART2
#define USE_USART3
#define USE_USART4
#include "HalStm32F0.h"

#elif defined(STM32F407xx) | defined (STM32F429xx)
#define LIB_MICRO_STM32F4
#define LIB_MICRO_STM32F
#define USE_USART1
#define USE_USART2
#define USE_USART3
#define USE_UART4
#define USE_UART5
#define USE_USART6
#include "HalStm32F4.h"

#elif defined(STM32L476xx)
#define LIB_MICRO_STM32L4
#define LIB_MICRO_STM32L
#define USE_USART1
#define USE_USART2
#define USE_USART3
#define USE_UART4
#define USE_UART5
#define USE_USART6
#include "HalStm32L4.h"

#elif defined(NRG1)
#define LIB_MICRO_NRG
#include "HalBlueNRG.h"

#elif defined(NRG2)
#define LIB_MICRO_NRG
#include "HalBlueNRG.h"

#endif

#endif // HAL_H
