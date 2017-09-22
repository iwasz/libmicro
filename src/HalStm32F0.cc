/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "HalStm32F0.h"
#include "Gpio.h"

extern "C" void EXTI0_1_IRQHandler ()
{
        if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_0) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_0);
                Gpio::connectedExtis[0]->onToggle ();
        }
        else if (__HAL_GPIO_EXTI_GET_IT (GPIO_PIN_1) != RESET) {
                __HAL_GPIO_EXTI_CLEAR_IT (GPIO_PIN_1);
                Gpio::connectedExtis[0]->onToggle ();
        }
}

extern "C" void EXTI2_3_IRQHandler () {}

extern "C" void EXTI4_15_IRQHandler () {}
