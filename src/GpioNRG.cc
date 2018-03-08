/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "Gpio.h"
#include <cstdint>
#include <cstring>

/*****************************************************************************/

// uint16_t Gpio::connectedExtisMask = 0;

Gpio *Gpio::connectedExtis[16] = {
        0,
};

/*****************************************************************************/

Gpio::Gpio (uint32_t pin, uint32_t mode, FunctionalState pull, FunctionalState hipower)
{
        GPIO_InitType gpioInitStructure;
        memset (&gpioInitStructure, 0, sizeof (gpioInitStructure));
        this->pin = pin;
        gpioInitStructure.GPIO_Pin = pin;
        gpioInitStructure.GPIO_Mode = mode;
        gpioInitStructure.GPIO_Pull = pull;
        gpioInitStructure.GPIO_HighPwr = hipower;
        SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_GPIO, ENABLE);
        GPIO_Init (&gpioInitStructure);
}

/*****************************************************************************/

Gpio::~Gpio ()
{
        int pinNo = pinNumber (pin);
        connectedExtis[pinNo] = nullptr;
}

/*****************************************************************************/

void Gpio::set (bool on)
{
        if (on) {
                GPIO->DATS = pin;
        }
        else {
                GPIO->DATC = pin;
        }
}

/*****************************************************************************/

void Gpio::setOnToggle (std::function<void(void)> const &t)
{
        int pinNo = pinNumber (pin);

        if (connectedExtis[pinNo]) {
                /*
                 * You cannot set a callback for this GPIO, because the pin it is configured for
                 * is already used as EXTI source. You can have only one Gpio  pin number configured
                 * as EXTI for example if you configure PA0 as an EXTI you cannot use any of PB0,
                 * PC0 and so on as EXTIS. See this conversation for example :
                 * https://stackoverflow.com/questions/23043465/find-specific-pin-on-which-a-interrupt-occured
                 */
                Error_Handler ();
        }

        connectedExtis[pinNo] = this;
        onToggle = t;
}

/*****************************************************************************/

int Gpio::pinNumber (uint32_t pinDef)
{
        int res = 0;
        while (pinDef >>= 1) {
                ++res;
        }

        return res;
}
