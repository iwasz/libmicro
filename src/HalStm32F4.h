/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef HALSTM32F0_H
#define HALSTM32F0_H

#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_i2c_ex.h>

#ifdef __cplusplus
extern "C" {
#endif

void EXTI0_IRQHandler ();
void EXTI1_IRQHandler ();
void EXTI2_IRQHandler ();
void EXTI3_IRQHandler ();
void EXTI4_IRQHandler ();
void EXTI9_5_IRQHandler ();
void EXTI15_10_IRQHandler ();

#define FRIEND_ALL_GPIO_IRQS                                                                                                                                   \
        friend void EXTI0_IRQHandler ();                                                                                                                       \
        friend void EXTI1_IRQHandler ();                                                                                                                       \
        friend void EXTI2_IRQHandler ();                                                                                                                       \
        friend void EXTI3_IRQHandler ();                                                                                                                       \
        friend void EXTI4_IRQHandler ();                                                                                                                       \
        friend void EXTI9_5_IRQHandler ();                                                                                                                     \
        friend void EXTI15_10_IRQHandler ();

#ifdef __cplusplus
}
#endif

#endif // HALSTM32F0_H
