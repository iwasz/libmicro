/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "Spi.h"
#include <cstring>

/*****************************************************************************/

Spi::Spi (uint8_t mode, uint32_t baudRate, uint8_t dataSize, uint8_t phase, uint8_t polarityClockSteadyState) : nssPin (nullptr)
{
        SPI_InitType spiInitStructure;
        memset (&spiInitStructure, 0, sizeof (spiInitStructure));

        SPI_StructInit (&spiInitStructure);
        spiInitStructure.SPI_Mode = mode;
        spiInitStructure.SPI_DataSize = dataSize;
        spiInitStructure.SPI_CPOL = polarityClockSteadyState;
        spiInitStructure.SPI_CPHA = phase;
        spiInitStructure.SPI_BaudRate = baudRate;

        SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_SPI, ENABLE);
        SPI_Init (&spiInitStructure);

        SPI_ClearTXFIFO ();
        SPI_ClearRXFIFO ();
        SPI_SetDummyCharacter (0xFF);
        SPI_SetMasterCommunicationMode (SPI_FULL_DUPLEX_MODE);

        // Enable SPI functionality
        SPI_Cmd (ENABLE);
}

/*****************************************************************************/

Spi::Spi (Spi const &s) { this->nssPin = s.nssPin; }

/*****************************************************************************/

Spi::~Spi ()
{
        SPI_Cmd (DISABLE);
        SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_SPI, DISABLE);
}

/*****************************************************************************/

void Spi::transmit8 (uint8_t const *txData, uint16_t size, uint8_t *rxData, size_t bogoDelay, bool dataPacking)
{
#if 0
        SPI_TypeDef *spi = spiHandle.Instance;

        size_t txRemainig = size;
        size_t rxRemainig = (!rxData) ? (0) : (size);

#ifndef LIB_MICRO_STM32F4
        if (dataPacking && rxRemainig > 1) {
                // set fiforxthreshold according the reception data length: 16bit
                CLEAR_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
        }
        else {
                // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
        }
#endif

        bool txAllowed = true;

        if ((spiHandle.Instance->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE) {
                __HAL_SPI_ENABLE (&spiHandle);
        }

        // Based on HAL code.
        while (txRemainig || rxRemainig) {

                // Sending part. TXE true means, old data has been sent, and we can push more bytes.
                if (txAllowed && txRemainig && (spi->SR & SPI_FLAG_TXE)) {

#ifndef LIB_MICRO_STM32F4
                        if (dataPacking && txRemainig > 1) {
                                spi->DR = *((uint16_t *)txData);
                                txData += sizeof (uint16_t);
                                txRemainig -= 2;
                        }
                        else {
#endif
                                *(__IO uint8_t *)&spi->DR = (*txData++);
                                --txRemainig;
#ifndef LIB_MICRO_STM32F4
                        }
#endif
                        if (rxRemainig) {
                                // Next Data is a reception (Rx). Tx not allowed
                                txAllowed = false;
                        }
                }

                // Receive block.  When RXNE flag set, it means that there is new data.
                if (rxRemainig && (spi->SR & SPI_FLAG_RXNE)) {

#ifndef LIB_MICRO_STM32F4
                        if (dataPacking && rxRemainig > 1) {
                                *((uint16_t *)rxData) = spi->DR;
                                rxData += sizeof (uint16_t);
                                rxRemainig -= 2;

                                if (rxRemainig <= 1) {
                                        // Set treshold for 8bits. RXNE will be set when fifo has at lest 8 bits
                                        SET_BIT (spi->CR2, SPI_RXFIFO_THRESHOLD);
                                }
                        }
                        else {
#endif
                                (*(uint8_t *)rxData++) = *(__IO uint8_t *)&spi->DR;
                                --rxRemainig;
#ifndef LIB_MICRO_STM32F4
                        }
#endif
                        // Next Data is a Transmission (Tx). Tx is allowed
                        txAllowed = true;
                }

                for (size_t i = 0; i < bogoDelay; ++i) {
                        __asm("nop");
                }
        }

        // In 2way mode (I don't use 1way modes), when we haven't read, the OVR will  occur. So we clear it.
        if (!rxData) {
                clearOvr ();
        }
#endif
}
/*****************************************************************************/

void Spi::receive8 (uint8_t *rxData, uint16_t size, size_t bogoDelay)
{
        size_t rxRemainig = (!rxData) ? (0) : (size);

        bool txAllowed = true;

        // Based on HAL code.
        while (rxRemainig) {

                // Sending part. TXE true means, old data has been sent, and we can push more bytes.
                if (txAllowed && (SPI->SR & SPI_FLAG_TFE)) {
                        *(__IO uint8_t *)&SPI->DR = 0xff;
                        // Next Data is a reception (Rx). Tx not allowed
                        txAllowed = false;
                }

                // Receive block.  When RXNE flag set, it means that there is new data.
                if (SPI->SR & SPI_FLAG_RNE) {

                        (*(uint8_t *)rxData++) = *(__IO uint8_t *)&SPI->DR;
                        --rxRemainig;

                        // Next Data is a Transmission (Tx). Tx is allowed
                        txAllowed = true;
                }

                for (size_t i = 0; i < bogoDelay; ++i) {
                        __asm("nop");
                }
        }
}

/*****************************************************************************/

uint8_t Spi::transmit8 (uint8_t word)
{
        while (!(SPI->SR & SPI_FLAG_TFE))
                ;

        SPI->DR = word;

        while (!(SPI->SR & SPI_FLAG_RNE))
                ;

        uint8_t ret = SPI->DR;

        while (SPI->SR & SPI_FLAG_BSY)
                ;

        return ret;
}

/*****************************************************************************/

void Spi::clearOvr ()
{
#if 0
#ifndef LIB_MICRO_STM32F4
        SET_BIT (spiHandle.Instance->CR2, SPI_RXFIFO_THRESHOLD);
#endif
        volatile uint16_t tmp;

        while (spiHandle.Instance->SR & SPI_FLAG_RXNE) {
                tmp = *(__IO uint8_t *)&spiHandle.Instance->DR;
        }

        tmp = spiHandle.Instance->SR;
        (void)tmp;
#endif
}
