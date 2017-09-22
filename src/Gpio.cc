/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Gpio.h"
#include "ErrorHandler.h"

/*****************************************************************************/

Gpio::Gpio (GPIO_TypeDef *port, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t speed)
{
        this->port = port;
        gpioInitStructure.Pin = pin;
        gpioInitStructure.Mode = mode;
        gpioInitStructure.Pull = pull;
        gpioInitStructure.Speed = speed;
        HAL_GPIO_Init (port, &gpioInitStructure);
        clkEnable ();
}

/*****************************************************************************/

Gpio::~Gpio ()
{
        clkDisable ();
        HAL_GPIO_DeInit (port, gpioInitStructure.Pin);
}

/*****************************************************************************/

void Gpio::clkEnable (GPIO_TypeDef *port)
{
        if (port == GPIOA) {
                __HAL_RCC_GPIOB_CLK_ENABLE ();
        }
        else if (port == GPIOB) {
                __HAL_RCC_GPIOB_CLK_ENABLE ();
        }
        else if (port == GPIOC) {
                __HAL_RCC_GPIOC_CLK_ENABLE ();
        }
        else if (port == GPIOD) {
                __HAL_RCC_GPIOD_CLK_ENABLE ();
        }
        else if (port == GPIOE) {
                __HAL_RCC_GPIOE_CLK_ENABLE ();
        }
        else if (port == GPIOF) {
                __HAL_RCC_GPIOF_CLK_ENABLE ();
        }
        else {
                Error_Handler ();
        }
}

/*****************************************************************************/

void Gpio::clkDisable (GPIO_TypeDef *port)
{
        if (port == GPIOA) {
                __HAL_RCC_GPIOB_CLK_DISABLE ();
        }
        else if (port == GPIOB) {
                __HAL_RCC_GPIOB_CLK_DISABLE ();
        }
        else if (port == GPIOC) {
                __HAL_RCC_GPIOC_CLK_DISABLE ();
        }
        else if (port == GPIOD) {
                __HAL_RCC_GPIOD_CLK_DISABLE ();
        }
        else if (port == GPIOE) {
                __HAL_RCC_GPIOE_CLK_DISABLE ();
        }
        else if (port == GPIOF) {
                __HAL_RCC_GPIOF_CLK_DISABLE ();
        }
        else {
                Error_Handler ();
        }
}

/*****************************************************************************/

void Gpio::set (bool on)
{
        if (on) {
                port->BSRR = gpioInitStructure.Pin;
        }
        else {
                port->BRR = gpioInitStructure.Pin;
        }
}
