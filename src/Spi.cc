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

        setNss (true);
}

/*****************************************************************************/

uint8_t Spi::transmit8 (uint8_t word)
{
        // Doesn't work without it
        __HAL_SPI_ENABLE (&spiHandle);
        // CLEAR_BIT (spiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD_HF);

        // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
        SET_BIT (spiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD);

        // Wait for tx buffer to be empty
        while (!(spiHandle.Instance->SR & SPI_FLAG_TXE))
                ;

        //        spiHandle.Instance->DR = word;
        *(__IO uint8_t *)&spiHandle.Instance->DR = word;
        //        *(__IO uint8_t *)&spiHandle.Instance->DR = word;
        //        *(__IO uint8_t *)&spiHandle.Instance->DR = word;
        //        *(__IO uint8_t *)&spiHandle.Instance->DR = word;

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
