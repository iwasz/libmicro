/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_UART_H
#define LIB_MICRO_UART_H

#include "Hal.h"

class Uart {
public:
        Uart (USART_TypeDef *instance, uint32_t baudRate);
        ~Uart ();

        static void clkEnable (USART_TypeDef *instance);
        static void clkDisable (USART_TypeDef *instance);
        void clkEnable () { clkEnable (huart.Instance); }
        void clkDisable () { clkDisable (huart.Instance); }

        void transmit (const uint8_t *str, size_t len);

private:
        UART_HandleTypeDef huart;
};

#endif // UART_H
