/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_SPI_H
#define LIB_MICRO_SPI_H

#include "Gpio.h"
#include "Hal.h"

/**
 * @brief SPI master mode.
 */
class Spi {
public:
        Spi (SPI_TypeDef *spi, uint32_t mode = SPI_MODE_MASTER, uint32_t dataSize = SPI_DATASIZE_8BIT, uint32_t phase = SPI_PHASE_1EDGE,
             uint32_t polarity = SPI_POLARITY_LOW);

        ~Spi () { clkDisable (); }

        // Obsolete if transmit8 prove to be ok.
        void transmit (uint8_t const *pTxData, uint8_t *pRxData, uint16_t Size);
        //        void transmit1 (uint8_t const *pTxData, uint16_t Size);

        /**
         * @brief Transmit and receive at the same time in 8 bits mode.
         * If you are not interested in received data, simply leave rxData nullptr.
         * Warning! Remember to use satNss (false) before transfer, and setNss (true) after.
         * @param txData
         * @param size
         * @param rxData Leave nullptr if not interested.
         */
        void transmit8 (uint8_t const *txData, uint16_t size, uint8_t *rxData = nullptr, size_t bogoDelay = 0);

        void receive8 (uint8_t *rxData, uint16_t size, size_t bogoDelay = 0);

        /**
         * @brief Transmit one byte, and return what has been sent back.
         * Warning! Remember to use satNss (false) before transfer, and setNss (true) after.
         */
        uint8_t transmit8 (uint8_t word);

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

        void setNssGpio (Gpio *n) { nssPin = n; }
        void setNss (bool b) { nssPin->set (b); }

private:
        SPI_HandleTypeDef spiHandle;
        Gpio *nssPin;
};

#endif // SPI_H
