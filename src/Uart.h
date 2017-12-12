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

extern "C" void USART3_4_IRQHandler ();

class Uart {
public:
        enum { MAX_RX_BUFFER = 128 };

        Uart (USART_TypeDef *instance, uint32_t baudRate);
        ~Uart ();

        static void clkEnable (USART_TypeDef *instance);
        static void clkDisable (USART_TypeDef *instance);
        void clkEnable () { clkEnable (huart.Instance); }
        void clkDisable () { clkDisable (huart.Instance); }

        void transmit (const uint8_t *str, size_t len);
        void receive ();

private:
        UART_HandleTypeDef huart;
        int rxBufferObdPos;
        uint8_t *rxBufferObd;

        // TODO ????
        friend void USART3_4_IRQHandler ();
        static Uart *usart1;
        static Uart *usart2;
        static Uart *usart3;
        static Uart *usart4;
};

#endif // UART_H
