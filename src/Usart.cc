/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Usart.h"
#include "ErrorHandler.h"
#include "Hal.h"
#include <cstring>

Usart *Usart::usart1;
Usart *Usart::usart2;
Usart *Usart::usart3;
Usart *Usart::usart4;

/*****************************************************************************/

Usart::Usart (USART_TypeDef *instance, uint32_t baudRate)
{
        if (instance == USART1) {
                usart1 = this;
        }
        else if (instance == USART2) {
                usart2 = this;
        }
        else if (instance == USART3) {
                usart3 = this;
        }
        else if (instance == USART4) {
                usart4 = this;
        }

        memset (&huart, 0, sizeof (huart));
        huart.Instance = instance;
        huart.Init.BaudRate = baudRate;
        huart.Init.WordLength = UART_WORDLENGTH_8B;
        huart.Init.StopBits = UART_STOPBITS_1;
        huart.Init.Parity = UART_PARITY_NONE;
        huart.Init.Mode = UART_MODE_TX_RX;
        huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart.Init.OverSampling = UART_OVERSAMPLING_16;

        clkEnable ();

        if (HAL_UART_Init (&huart) != HAL_OK) {
                Error_Handler ();
        }
}

/*****************************************************************************/

Usart::~Usart ()
{
        clkDisable ();
}

/*****************************************************************************/

void Usart::clkEnable (USART_TypeDef *instance)
{
        if (instance == USART1) {
                __HAL_RCC_USART1_CLK_ENABLE ();
        }
        else if (instance == USART2) {
                __HAL_RCC_USART2_CLK_ENABLE ();
        }
        else if (instance == USART3) {
                __HAL_RCC_USART3_CLK_ENABLE ();
        }
        else if (instance == USART4) {
                __HAL_RCC_USART4_CLK_ENABLE ();
        }
}

/*****************************************************************************/

void Usart::clkDisable (USART_TypeDef *instance)
{
        if (instance == USART1) {
                __HAL_RCC_USART1_CLK_DISABLE ();
        }
        else if (instance == USART2) {
                __HAL_RCC_USART2_CLK_DISABLE ();
        }
        else if (instance == USART3) {
                __HAL_RCC_USART3_CLK_DISABLE ();
        }
        else if (instance == USART4) {
                __HAL_RCC_USART4_CLK_DISABLE ();
        }
}

/*****************************************************************************/

void Usart::transmit (const uint8_t *str, size_t len) { HAL_UART_Transmit (&huart, const_cast<uint8_t *> (str), len, 5000); }

/*****************************************************************************/

void Usart::startReceive (std::function<void(uint8_t)> const &t)
{
        onData = t;

        // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
        huart.Instance->CR3 |= USART_CR3_EIE;

        // Enable the UART Parity Error and Data Register not empty Interrupts
        huart.Instance->CR1 |= (USART_CR1_PEIE | USART_CR1_RXNEIE);
}

/*****************************************************************************/

void Usart::stopReceive ()
{
        // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
        huart.Instance->CR3 &= ~USART_CR3_EIE;

        // Enable the UART Parity Error and Data Register not empty Interrupts
        huart.Instance->CR1 &= ~(USART_CR1_PEIE | USART_CR1_RXNEIE);
}

/*****************************************************************************/

void Usart::fireOnData (Usart *u)
{
        // If it were not initialized
        if (!u) {
                return;
        }

        UART_HandleTypeDef *huart = &u->huart;

        uint32_t isrflags = READ_REG (huart->Instance->ISR);
        uint32_t cr1its = READ_REG (huart->Instance->CR1);
        uint32_t errorflags;

        errorflags = (isrflags & (uint32_t) (USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
        if (errorflags) {
                Error_Handler ();
        }

        if (((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
                uint8_t c = (uint8_t) (huart->Instance->RDR & (uint8_t)0x00FF);
                u->onData (c);
        }
}

/*****************************************************************************/

extern "C" void USART1_IRQHandler () { Usart::fireOnData (Usart::usart1); }

/*****************************************************************************/

extern "C" void USART2_IRQHandler () { Usart::fireOnData (Usart::usart2); }

/*****************************************************************************/

extern "C" void USART3_4_IRQHandler ()
{
        Usart::fireOnData (Usart::usart3);
        Usart::fireOnData (Usart::usart4);
}
