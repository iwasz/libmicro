/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Nrf24L01P.h"
#include <cstring>

/*****************************************************************************/

Nrf24L01P::Nrf24L01P (Spi *spi, Gpio *cePin, Gpio *irqPin) : spi (spi), cePin (cePin), irqPin (irqPin)
{
        if (irqPin) {
                irqPin->setOnToggle ([this] {
                        writeRegister (Nrf24L01P::STATUS, RX_DR);

                        if (onData) {
                                onData ();
                        }
                });
        }
}

/*****************************************************************************/

void Nrf24L01P::setConfig (uint8_t maskIrqSource, bool crcEnable, CrcLength crcLength)
{
        configRegisterCopy &= (1 << PWR_UP | 1 << PRIM_RX);
        configRegisterCopy |= maskIrqSource | (uint8_t (crcEnable) << EN_CRC) | crcLength;
        writeRegister (CONFIG, configRegisterCopy);
}

/*****************************************************************************/

void Nrf24L01P::writeRegister (uint8_t reg, uint8_t value)
{
        // TODO delete this
        uint8_t dummy[2];
        uint8_t buf[2];
        buf[0] = reg | W_REGISTER;
        buf[1] = value;
        spi->transmit (buf, dummy, 2);

        // TODO use 2 spi->trasmit calls. First with reg-address, and second with value.
}

// void Nrf24L01P::writeCommand (uint8_t command, uint8_t *data, uint8_t len)
//{
//        uint8_t dummy[2];
//        uint8_t buf[2];
//        buf[0] = command;
//        buf[1] = value;
//        spi->transmit (buf, dummy, 2);

//        // TODO use 2 spi->trasmit calls. First with reg-address, and second with value.
//}

/*****************************************************************************/

void Nrf24L01P::writeRegister (uint8_t reg, uint8_t const *data, uint8_t len)
{
        uint8_t dummy[6];
        uint8_t buf[6];
        buf[0] = reg | W_REGISTER;
        memcpy (buf + 1, data, len);
        spi->transmit (buf, dummy, len + 1);
}

/*****************************************************************************/

uint8_t Nrf24L01P::readRegister (uint8_t reg) const
{
        uint8_t bufRx[2];
        uint8_t bufTx[2];
        bufTx[0] = reg | R_REGISTER;
        bufTx[1] = NOP;
        spi->transmit (bufTx, bufRx, 2);
        return bufRx[1];
}

/*****************************************************************************/

void Nrf24L01P::powerUp (Mode mode)
{
        configRegisterCopy |= (1 << PWR_UP) | mode;
        writeRegister (CONFIG, configRegisterCopy);

        // Start listenig right away
        if (mode == RX) {
                setCe (true);
        }

        // Clear FIFOS
        uint8_t bufTx = FLUSH_TX;
        uint8_t bufRx;
        spi->transmit (&bufTx, &bufRx, 1);

        bufTx = FLUSH_RX;
        spi->transmit (&bufTx, &bufRx, 1);

        // Clear IRQS
        writeRegister (STATUS, 0x70);
}

void Nrf24L01P::flushTx ()
{
        uint8_t bufTx = FLUSH_TX;
        uint8_t bufRx;
        spi->transmit (&bufTx, &bufRx, 1);
}

void Nrf24L01P::flushRx ()
{
        uint8_t bufTx = FLUSH_RX;
        uint8_t bufRx;
        spi->transmit (&bufTx, &bufRx, 1);
}

/*****************************************************************************/

void Nrf24L01P::transmit (uint8_t *data, size_t len)
{
        setCe (true);
        uint8_t dummy[33];
        uint8_t dummyRx[33];
        dummy[0] = W_TX_PAYLOAD;
        memcpy (dummy + 1, data, len);
        spi->transmit (dummy, dummyRx, len + 1);
        HAL_Delay (1);
        setCe (false);
}

/*****************************************************************************/

uint8_t *Nrf24L01P::receive (uint8_t *data, size_t len)
{
        static uint8_t tmp[33] = {
                R_RX_PAYLOAD,
                0,
        };

        spi->transmit (tmp, data, len + 1);
        return data + 1;
}
