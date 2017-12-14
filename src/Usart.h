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
#include <functional>

extern "C" void USART1_IRQHandler ();
extern "C" void USART2_IRQHandler ();
extern "C" void USART3_4_IRQHandler ();

/**
 * @brief The Uart class
 */
class Usart {
public:
        Usart (USART_TypeDef *instance, uint32_t baudRate);
        ~Usart ();

        static void clkEnable (USART_TypeDef *instance);
        static void clkDisable (USART_TypeDef *instance);
        void clkEnable () { clkEnable (huart.Instance); }
        void clkDisable () { clkDisable (huart.Instance); }

        void transmit (const uint8_t *str, size_t len);
        void startReceive (std::function<void(uint8_t)> const &t);
        void stopReceive ();

private:
        UART_HandleTypeDef huart;

        // TODO ????
        friend void USART1_IRQHandler ();
        friend void USART2_IRQHandler ();
        friend void USART3_4_IRQHandler ();
        static Usart *usart1;
        static Usart *usart2;
        static Usart *usart3;
        static Usart *usart4;
        static void fireOnData (Usart *u);
        std::function <void (uint8_t)> onData;
};

#endif // UART_H
