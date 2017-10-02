/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_SPI_H
#define LIB_MICRO_SPI_H

#include "Hal.h"
#include <functional>

class Gpio;

class Spi {
public:
        Spi (SPI_TypeDef *spi, uint32_t mode = SPI_MODE_MASTER, uint32_t dataSize = SPI_DATASIZE_8BIT, uint32_t phase = SPI_PHASE_1EDGE,
             uint32_t polarity = SPI_POLARITY_LOW);

        ~Spi () { clkDisable (); }

        void transmit (uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
//        uint16_t transmit (uint16_t word);

        //        void setCallback (std::function<void(uint8_t)> const &f) { fff = f; }

        /**
         * @brief clkEnable Runs __HAL_RCC_SPIx_CLK_ENABLE for a SPIx.
         * @param gpiox The port.
         */
        static void clkEnable (SPI_HandleTypeDef *spiX);

        /**
         * @brief clkEnable Runs __HAL_RCC_SPIx_CLK_ENABLE for a SPIx.
         * @param gpiox The port.
         */
        static void clkDisable (SPI_HandleTypeDef *spiX);

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_ENABLE macro for this Gpio.
         */
        void clkEnable () { clkEnable (&spiHandle); }

        /**
         * @brief clkEnable Runs appropriate __HAL_RCC_GPIOx_DISABLE macro for this Gpio.
         */
        void clkDisable () { clkDisable (&spiHandle); }

        void setNssPin (Gpio *n) { nssPin = n; }

private:
        SPI_HandleTypeDef spiHandle;
        //        std::function<void(uint8_t)> fff;
        Gpio *nssPin;
};

#endif // SPI_H
