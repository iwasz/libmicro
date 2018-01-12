/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Nrf24L01P.h"
#include "Debug.h"
#include <cstring>

/*****************************************************************************/

Nrf24L01P::Nrf24L01P (Spi *spi, Gpio *cePin, Gpio *irqPin)
    : spi (spi), cePin (cePin), irqPin (irqPin), configRegisterCopy (0x08), callback (nullptr)
{
        if (irqPin) {
                irqPin->setOnToggle ([this] {
                        uint8_t s = getStatus ();

                        /*
                         * From the datasheet:
                         * The RX_DR IRQ is asserted by a new packet arrival event. The procedure for handling this interrupt should
                         * be: 1) read payload through SPI, 2) clear RX_DR IRQ, 3) read FIFO_STATUS to check if there are more
                         * payloads available in RX FIFO, 4) if there are more data in RX FIFO, repeat from step 1).
                         *
                         * Note: Always check if the packet width reported is 32 bytes or shorter when using the R_RX_PL_WID command. If its
                         * width is longer than 32 bytes then the packet contains errors and must be discarded. Discard the packet by using the
                         * Flush_RX command.
                         */
                        if (s & RX_DR) {
                                // readFifostatus
                                while (!(readRegister (Nrf24L01P::FIFO_STATUS) & RX_EMPTY)) {
                                        size_t payloadLen = getPayloadLength ();

                                        if (payloadLen > 32 || payloadLen <= 0) {
                                                flushRx ();
                                                writeRegister (Nrf24L01P::STATUS, RX_DR);
                                                return;
                                        }

                                        uint8_t *out = receive (bufferRx, payloadLen);
                                        writeRegister (Nrf24L01P::STATUS, RX_DR);

                                        if (callback) {
                                                callback->onRx (out, payloadLen);
                                        }
                                }
                        }

                        /*
                         * Data Sent TX FIFO interrupt. Asserted when packet transmitted on TX. If AUTO_ACK is activated, this bit is set high
                         * only when ACK is received. Write 1 to clear bit.
                         */
                        if (s & TX_DS) {
                                // TODO to się wykonuje nawet gdy jest niepotrzebne :(
                                if (callback) {
                                        callback->onTx ();
                                }

                                writeRegister (Nrf24L01P::STATUS, TX_DS);
                        }

                        /*
                         * Maximum number of TX retransmits interrupt Write 1 to clear bit. If MAX_RT is asserted it must be cleared to enable
                         * further communication.
                         */
                        if (s & MAX_RT) {
                                if (callback) {
                                        callback->onMaxRt ();
                                }

                                writeRegister (Nrf24L01P::STATUS, MAX_RT);
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

/*****************************************************************************/

void Nrf24L01P::flushTx ()
{
        uint8_t bufTx = FLUSH_TX;
        uint8_t bufRx;
        spi->transmit (&bufTx, &bufRx, 1);
}

/*****************************************************************************/

void Nrf24L01P::flushRx ()
{
        uint8_t bufTx = FLUSH_RX;
        uint8_t bufRx;
        spi->transmit (&bufTx, &bufRx, 1);
}

/*****************************************************************************/

void Nrf24L01P::transmit (uint8_t *data, size_t len, bool noAck)
{
        // TODO zoptymalizować, te metody są bez sensu, memcpy jest bez sensu, przeciez można wysyłać bezpośrednio data i nie potrzeba dummyRx
        setCe (true);
        uint8_t dummy[33];
        uint8_t dummyRx[33];

        if (noAck) {
                dummy[0] = W_TX_PAYLOAD_NO_ACK;
        }
        else {
                dummy[0] = W_TX_PAYLOAD;
        }

        memcpy (dummy + 1, data, len);
        spi->transmit (dummy, dummyRx, len + 1);
        setCe (false);
}

/*****************************************************************************/

void Nrf24L01P::setAckPayload (uint8_t forPipe, uint8_t *data, size_t len)
{
        // TODO zoptymalizować, te metody są bez sensu, memcpy jest bez sensu, przeciez można wysyłać bezpośrednio data i nie potrzeba dummyRx
        //        setCe (true);
        uint8_t dummy[33];
        uint8_t dummyRx[33];
        dummy[0] = W_ACK_PAYLOAD | forPipe;
        memcpy (dummy + 1, data, len);
        spi->transmit (dummy, dummyRx, len + 1);
        //        setCe (false);
}

/*****************************************************************************/

size_t Nrf24L01P::getPayloadLength () const
{
        uint8_t in[2], out[2];
        in[0] = R_RX_PL_WID;
        spi->transmit (in, out, 2);
        return out[1];
}

/*****************************************************************************/

uint8_t *Nrf24L01P::receive (uint8_t *data, size_t len)
{
        static uint8_t tmp[33] = {
                R_RX_PAYLOAD,
                0,
        };

        // TODO źle! W ten sposób wrzucamy do data len+1, a on będzie miał przecież tylk len!
        spi->transmit (tmp, data, len + 1);
        return data + 1;
}

/*****************************************************************************/

void Nrf24L01P::poorMansScanner (int tries)
{
        uint8_t channel[CHANNELS_NO_FOR_SANNER];
        memset (channel, 0, CHANNELS_NO_FOR_SANNER);
        Debug *d = Debug::singleton ();

        for (int j = 0; j < tries; ++j) {
                for (int i = 0; i < CHANNELS_NO_FOR_SANNER; ++i) {
                        // select a new channel
                        // setChannel ((128 * i) / CHANNELS_NO_FOR_SANNER);
                        setChannel (i);

                        // switch on RX
                        powerUp (RX);

                        // wait enough for RX-things to settle
                        HAL_Delay (1);

                        // this is actually the point where the RPD-flag
                        // is set, when CE goes low
                        setCe (false);

                        // read out RPD flag; set to 1 if
                        // received power > -64dBm
                        uint8_t rpd = readRegister (RPD);

                        if (rpd > 0) {
                                ++channel[i];
                        }

                        //                        HAL_Delay (1);
                }
        }

        for (int i = 0; i < CHANNELS_NO_FOR_SANNER; ++i) {
                d->print (i);
                d->print (" : ");

                for (int j = 0; j < channel[i]; ++j) {
                        d->print ("#");
                }

                d->print ("\n");
        }
}
