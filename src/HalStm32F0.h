/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef HALSTM32F0_H
#define HALSTM32F0_H

#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_flash_ex.h>

#ifdef __cplusplus
extern "C" void EXTI0_1_IRQHandler ();
extern "C" void EXTI2_3_IRQHandler ();
extern "C" void EXTI4_15_IRQHandler ();

extern "C" void USART1_IRQHandler ();
extern "C" void USART2_IRQHandler ();
extern "C" void USART3_4_IRQHandler ();

#define FRIEND_ALL_GPIO_IRQS                                                                                                                                   \
        friend void EXTI0_1_IRQHandler ();                                                                                                                     \
        friend void EXTI2_3_IRQHandler ();                                                                                                                     \
        friend void EXTI4_15_IRQHandler ();

#define FRIEND_ALL_USART_IRQS                                                                                                                                  \
        friend void USART1_IRQHandler ();                                                                                                                      \
        friend void USART2_IRQHandler ();                                                                                                                      \
        friend void USART3_4_IRQHandler ();

#endif

#endif // HALSTM32F0_H
