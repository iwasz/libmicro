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

Uart::Uart (USART_TypeDef *instance, uint32_t baudRate)
{
        memset (&huart, 0, sizeof (huart));
        huart.Instance = instance;
        huart.Init.BaudRate = baudRate;
        huart.Init.WordLength = UART_WORDLENGTH_8B;
        huart.Init.StopBits = UART_STOPBITS_1;
        huart.Init.Parity = UART_PARITY_NONE;
        huart.Init.Mode = UART_MODE_TX_RX;
        huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart.Init.OverSampling = UART_OVERSAMPLING_16;
        //        huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        //        huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

        __HAL_RCC_USART3_CLK_ENABLE ();
        if (HAL_UART_Init (&huart) != HAL_OK) {
                Error_Handler ();
        }
}

/*****************************************************************************/

Uart::~Uart () { clkDisable (); }

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
