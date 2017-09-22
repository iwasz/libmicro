/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_GPIO_H
#define LIB_MICRO_GPIO_H

#include <Hal.h>
#include <functional>

/**
 * @brief GPIO port adapter class. RAII.
 */
class Gpio {
public:
        /**
         * @brief Initializes, and turns the clock on.
         */
        Gpio (GPIO_TypeDef *port, uint32_t pin, uint32_t mode = GPIO_MODE_OUTPUT_PP, uint32_t pull = GPIO_NOPULL, uint32_t speed = GPIO_SPEED_FREQ_HIGH);
        ~Gpio ();

        /**
         * @brief clkEnable Runs __HAL_RCC_GPIOx_ENABLE for a GPIOx port.
         * @param gpiox The port.
         */
        static void clkEnable (GPIO_TypeDef *port);

        /**
         * @brief clkEnable Runs __HAL_RCC_GPIOx_ENABLE for a GPIOx port.
         * @param gpiox The port.
         */
        static void clkDisable (GPIO_TypeDef *port);

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_ENABLE macro for this Gpio.
         */
        void clkEnable () { clkEnable (port); }

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_DISABLE macro for this Gpio.
         */
        void clkDisable () { clkDisable (port); }

        /**
         * @brief Turns on or off.
         * @param on
         */
        void set (bool on);

        void setOnToggle (std::function<void(void)> const &t) { onToggle = t; }

private:
        GPIO_InitTypeDef gpioInitStructure;
        GPIO_TypeDef *port;
        // Serviced in EXTI ISR
        std::function<void(void)> onToggle;
};

#endif // GPIO_H
