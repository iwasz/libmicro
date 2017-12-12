/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Uart.h"
#include "ErrorHandler.h"
#include "Hal.h"
#include <cstring>

Uart *Uart::usart1;
Uart *Uart::usart2;
Uart *Uart::usart3;
Uart *Uart::usart4;

Uart::Uart (USART_TypeDef *instance, uint32_t baudRate) : rxBufferObdPos (0)
{
        if (instance == USART3) {
                usart3 = this;
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

Uart::~Uart ()
{
        clkDisable ();
        delete[] rxBufferObd;
}

/*****************************************************************************/

void Uart::clkEnable (USART_TypeDef *instance)
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

void Uart::clkDisable (USART_TypeDef *instance)
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

void Uart::transmit (const uint8_t *str, size_t len) { HAL_UART_Transmit (&huart, const_cast<uint8_t *> (str), len, 5000); }

/*****************************************************************************/

void Uart::receive ()
{
        rxBufferObd = new uint8_t[MAX_RX_BUFFER];

        // Enable the UART Error Interrupt: (Frame error, noise error, overrun error)
        huart.Instance->CR3 |= USART_CR3_EIE;

        // Enable the UART Parity Error and Data Register not empty Interrupts
        huart.Instance->CR1 |= (USART_CR1_PEIE | USART_CR1_RXNEIE);
}

/*****************************************************************************/

extern "C" void USART3_4_IRQHandler ()
{
        UART_HandleTypeDef *huart = &Uart::usart3->huart;

        uint32_t isrflags = READ_REG (huart->Instance->ISR);
        uint32_t cr1its = READ_REG (huart->Instance->CR1);
        uint32_t cr3its;
        uint32_t errorflags;

        errorflags = (isrflags & (uint32_t) (USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
        if (errorflags) {
                Error_Handler ();
        }

        if (((isrflags & USART_ISR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET)) {
//                UART_Receive_IT (huart);

                uint8_t c = (uint8_t)(huart->Instance->RDR & (uint8_t)0x00FF);
                Uart::usart3->transmit (&c, 1);
                return;
        }
}
