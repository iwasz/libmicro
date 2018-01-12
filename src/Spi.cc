/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Spi.h"
#include "ErrorHandler.h"
#include <cstring>

/*****************************************************************************/

Spi::Spi (SPI_TypeDef *spi, uint32_t mode, uint32_t dataSize, uint32_t phase, uint32_t polarity) : nssPin (nullptr)
{
        memset (&spiHandle, 0, sizeof (spiHandle));
        spiHandle.Instance = spi;
        spiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
        spiHandle.Init.Direction = SPI_DIRECTION_2LINES;
        spiHandle.Init.CLKPhase = phase;
        spiHandle.Init.CLKPolarity = polarity;
        spiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        spiHandle.Init.CRCPolynomial = 7;
        spiHandle.Init.DataSize = dataSize;
        spiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
        spiHandle.Init.NSS = SPI_NSS_SOFT;
#ifdef LIB_MICRO_STM32F0
        spiHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
#endif
        spiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
        spiHandle.Init.Mode = mode;

        clkEnable ();
        HAL_SPI_Init (&spiHandle);
}

/*****************************************************************************/

void Spi::transmit (uint8_t const *txData, uint8_t *rxData, uint16_t size)
{
        setNss (false);

        if (HAL_SPI_TransmitReceive (&spiHandle, const_cast<uint8_t *> (txData), rxData, size, 500) != HAL_OK) {
                Error_Handler ();
        }
        // HAL_SPI_Transmit()
        setNss (true);
}

//void Spi::transmit1 (uint8_t const *txData, uint16_t size)
//{
//        setNss (false);

//        if (HAL_SPI_Transmit (&spiHandle, const_cast<uint8_t *> (txData), size, 500) != HAL_OK) {
//                Error_Handler ();
//        }

//        setNss (true);
//}

/*****************************************************************************/

void Spi::transmit8 (uint8_t const *txData, uint16_t size, uint8_t *rxData)
{
        SPI_TypeDef *spi = spiHandle.Instance;

        size_t txRemainig = (!txData) ? (0) : (size);
        size_t rxRemainig = (!rxData) ? (0) : (size);
        // size_t txRemainig = size;
        // size_t rxRemainig = size;

        if (rxRemainig > 1) {
                // set fiforxthreshold according the reception data length: 16bit
                CLEAR_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
        }
        else {
                // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
        }

        bool txAllowed = true;

        if ((spiHandle.Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
                __HAL_SPI_ENABLE (&spiHandle);
        }

        // Based on HAL code.
        while (txRemainig || rxRemainig) {

                // Sending part. TXE true means, old data has been sent, and we can push more bytes.
                if (txAllowed && txRemainig && (spi->SR & SPI_FLAG_TXE)) {

                        if (txRemainig > 1) {
                                spi->DR = *((uint16_t *)txData);
                                txData += sizeof (uint16_t);
                                txRemainig -= 2;
                        }
                        else {
                                *(__IO uint8_t *)&spi->DR = (*txData++);
                                --txRemainig;
                        }

                        if (rxRemainig) {
                                // Next Data is a reception (Rx). Tx not allowed
                                txAllowed = false;
                        }
                        else {
                                HAL_Delay(1);
                        }
                }

                // Receive block.  When RXNE flag set, it means that there is new data.
                if (rxRemainig && (spi->SR & SPI_FLAG_RXNE)) {

                        if (rxRemainig > 1) {
                                // TODO nie umiem bez odczytywania.
                                uint16_t tmp = spi->DR;

                                if (rxData) {
                                        *((uint16_t *)rxData) = tmp;
                                        rxData += sizeof (uint16_t);
                                }

                                rxRemainig -= 2;

                                if (rxRemainig <= 1) {
                                        // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                                        SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
                                }
                        }
                        else {
                                uint8_t tmp = *(__IO uint8_t *)&spi->DR;

                                if (rxData) {
                                        (*(uint8_t *)rxData++) = tmp;
                                }
                                --rxRemainig;
                        }

                        // Next Data is a Transmission (Tx). Tx is allowed
                        txAllowed = true;
                }
        }

        // In 2way mode (I don't use 1way modes), when we haven't read, the OVR will  occur. So we clear it.
        if (!rxData) {
                CLEAR_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
                volatile uint16_t tmp = spi->DR;
                tmp = spi->SR;
                tmp = spi->DR;
                tmp = spi->SR;
                __HAL_SPI_CLEAR_OVRFLAG (&spiHandle);
        }
}

/*****************************************************************************/

uint8_t Spi::transmit8 (uint8_t word)
{
        // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
        SET_BIT (spiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD);

        // Doesn't work without it
        if ((spiHandle.Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
                __HAL_SPI_ENABLE (&spiHandle);
        }

        // Wait for tx buffer to be empty
        while (!(spiHandle.Instance->SR & SPI_FLAG_TXE))
                ;

        *(__IO uint8_t *)&spiHandle.Instance->DR = word;

        while (!(spiHandle.Instance->SR & SPI_FLAG_RXNE))
                ;

        return *(__IO uint8_t *)&spiHandle.Instance->DR;
}

/*****************************************************************************/

void Spi::clkEnable (SPI_HandleTypeDef *spiX)
{
        if (spiX->Instance == SPI1) {
                __HAL_RCC_SPI1_CLK_ENABLE ();
        }
        else if (spiX->Instance == SPI2) {
                __HAL_RCC_SPI2_CLK_ENABLE ();
        }

        __HAL_SPI_ENABLE (spiX);
}

/*****************************************************************************/

void Spi::clkDisable (SPI_HandleTypeDef *spiX)
{
        if (spiX->Instance == SPI1) {
                __HAL_RCC_SPI1_CLK_DISABLE ();
        }
        else if (spiX->Instance == SPI2) {
                __HAL_RCC_SPI2_CLK_DISABLE ();
        }

        __HAL_SPI_DISABLE (spiX);
}
