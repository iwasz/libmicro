/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Spi.h"
#include "ErrorHandler.h"
#include "Gpio.h"

Spi::Spi (SPI_TypeDef *spi, uint32_t mode, uint32_t dataSize, uint32_t phase, uint32_t polarity)
{
        spiHandle.Instance = spi;
        spiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
        spiHandle.Init.Direction = SPI_DIRECTION_2LINES;
        spiHandle.Init.CLKPhase = phase;
        spiHandle.Init.CLKPolarity = polarity;
        spiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        spiHandle.Init.CRCPolynomial = 7;
        spiHandle.Init.DataSize = dataSize;
        spiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
        spiHandle.Init.NSS = SPI_NSS_SOFT;
        spiHandle.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
        spiHandle.Init.TIMode = SPI_TIMODE_DISABLE;
        spiHandle.Init.Mode = mode;

        clkEnable ();
        HAL_SPI_Init (&spiHandle);
}

/*****************************************************************************/

void Spi::transmit (uint8_t *txData, uint8_t *rxData, uint16_t size)
{
        nssPin->set (false);

        if (HAL_SPI_TransmitReceive (&spiHandle, txData, rxData, size, 500) != HAL_OK) {
                Error_Handler ();
        }

        nssPin->set (true);
}

/*****************************************************************************/

uint16_t Spi::transmit (uint16_t word)
{
        nssPin->set (false);

        __HAL_SPI_ENABLE (&spiHandle);
        CLEAR_BIT (spiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD);

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

        nssPin->set (true);
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
