/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef NRF24L0P_H
#define NRF24L0P_H

#include "Gpio.h"
#include "Spi.h"
#include <functional>

/**
 * States:
 * - Power Down (default after turning on).
 * - Standby 1 (CONFIG:PWR_UP bit set)
 * - RX Mode (it stays in it when CONFIG:PRIM_RX is set)
 * - TX Mode
 *
 * RX : constantly listens to valid packets (i.e. correct structure and correct address), and puts the data into FIFO.
 * If fifo is full, it discards new packets.
 *
 * TX :
 *
 * Various info:
 * - This "Enhanced ShockBurst" thing is ON by default (?)
 * - Static payload length is the default.
 * - An ACK packet can contain an optional payload from PRX to PTX. In order to use this feature, the
 * Dynamic Payload Length (DPL) feature must be enabled. W_ACK_PAYLOAD is used for this.
 */
class Nrf24L01P {
public:
        Nrf24L01P (Spi *spi, Gpio *cePin, Gpio *irqPin);

        enum ConfigBit { EN_CRC = 3, CRCO = 2, PWR_UP = 1, PRIM_RX = 0 };
        enum MaskIrqSource {
                MASK_RX_DR = 1 << 6, // Block RX IRQ
                MASK_TX_DS = 1 << 5, // Block TX IRQ
                MASK_MAX_RT = 1 << 4,
                MASK_ALL_IRQ = MASK_RX_DR | MASK_TX_DS | MASK_MAX_RT,
                MASK_NO_IRQ = 0
        };
        enum CrcLength { CRC_LEN_1 = 0, CRC_LEN_2 = 1 << CRCO };
        void setConfig (uint8_t maskIrqSource, bool crcEnable, CrcLength crcLength);

        enum Mode { TX, RX };
        void powerUp (Mode mode);

        enum EnAa { ENAA_P5 = 1 << 5, ENAA_P4 = 1 << 4, ENAA_P3 = 1 << 3, ENAA_P2 = 1 << 2, ENAA_P1 = 1 << 1, ENAA_P0 = 1 << 0 };
        void setAutoAck (uint8_t enAa) { writeRegister (EN_AA, enAa); }

        enum EnRxAddr { ERX_P5 = 1 << 5, ERX_P4 = 1 << 4, ERX_P3 = 1 << 3, ERX_P2 = 1 << 2, ERX_P1 = 1 << 1, ERX_P0 = 1 << 0 };
        void setEnableDataPipe (uint8_t pipes) { writeRegister (EN_RXADDR, pipes); }

        enum AddressWidth { WIDTH_3 = 0x01, WIDH_4 = 0x02, WIDTH_5 = 0x03 };
        void setAdressWidth (AddressWidth aw) { writeRegister (SETUP_AW, aw); }

        enum RetransmitDelay {
                WAIT_250,
                WAIT_500,
                WAIT_750,
                WAIT_1000,
                WAIT_1250,
                WAIT_1500,
                WAIT_1750,
                WAIT_2000,
                WAIT_2250,
                WAIT_2500,
                WAIT_2750,
                WAIT_3000,
                WAIT_3250,
                WAIT_3500,
                WAIT_3750,
                WAIT_4000
        };

        enum RetransmitCount {
                RETRANSMIT_0,
                RETRANSMIT_1,
                RETRANSMIT_2,
                RETRANSMIT_3,
                RETRANSMIT_4,
                RETRANSMIT_5,
                RETRANSMIT_6,
                RETRANSMIT_7,
                RETRANSMIT_8,
                RETRANSMIT_9,
                RETRANSMIT_10,
                RETRANSMIT_11,
                RETRANSMIT_12,
                RETRANSMIT_13,
                RETRANSMIT_14,
                RETRANSMIT_15
        };

        void setAutoRetransmit (RetransmitDelay d, RetransmitCount c) { writeRegister (SETUP_RETR, d | c); }

        void setChannel (uint8_t channel) { writeRegister (RF_CH, channel); }

        enum DataRate { MBPS_1 = 0 , MBPS_2 = 1 << 3, KBPS_250 = 1 << 5 };
        enum Gain { DBM_18 = 0 << 1, DBM_12 = 1 << 1, DBM_6 = 2 << 1, DBM_0 = 3 << 1 };
        void setDataRate (DataRate dr, Gain g) { writeRegister (RF_SETUP, dr | g); }

        enum IrqSource { RX_DR = 1 << 6, TX_DS = 1 << 5, MAX_RT = 1 << 4, IRQ_ALL = RX_DR | TX_DS | MAX_RT };
        uint8_t getStatus () const { return readRegister (STATUS); }

        void getObserve (uint8_t *lostPackets, uint8_t *retransmittedPackets) const
        {
                *lostPackets = readRegister (OBSERVE_TX);
                *retransmittedPackets = *lostPackets & 0x0f;
                *lostPackets >>= 4;
        }

        bool getReceivedPowerDetector () const { return readRegister (RPD) & 1; }
        void setRxAddress (uint8_t dataPipeNo, uint8_t const *address, uint8_t addressLen) { writeRegister (RX_ADDR_P0 + dataPipeNo, address, addressLen); }
        void setTxAddress (uint8_t const *address, uint8_t addressLen) { writeRegister (TX_ADDR, address, addressLen); }

        enum DynamicPayloadLength { DPL_P5 = 1 << 5, DPL_p4 = 1 << 4, DPL_P3 = 1 << 3, DPL_P2 = 1 << 2, DPL_P1 = 1 << 1, DPL_P0 = 1 << 0 };
        void setPayloadLength (uint8_t dataPipeNo, uint8_t len) { writeRegister (RX_PW_P0 + dataPipeNo, len); }

        /// Mask with something
        uint8_t getFifoStatus () const { return readRegister (FIFO_STATUS); }

        /// DynamicPayloadLength
        void setEnableDynamicPayload (uint8_t dpl) { writeRegister (DYNPD, dpl); }

        /// Feature
        void setFeature (uint8_t f) { writeRegister (FEATURE, f); }

        void transmit (uint8_t *data, size_t len);
        void receive (uint8_t *data, size_t len);

        void setOnData (std::function<void(void)> const &t) { onData = t; }

private:
        void writeRegister (uint8_t reg, uint8_t value);
        void writeRegister (uint8_t reg, uint8_t const *data, uint8_t len);
        uint8_t readRegister (uint8_t reg) const;
        void setCe (bool b) { cePin->set (b); }

        enum Register {
                CONFIG = 0x00,
                EN_AA = 0x01,
                EN_RXADDR = 0x02,
                SETUP_AW = 0x03,
                SETUP_RETR = 0x04,
                RF_CH = 0x05,
                RF_SETUP = 0x06,
                STATUS = 0x07,
                OBSERVE_TX = 0x08,
                RPD = 0x09,
                RX_ADDR_P0 = 0x0A,
                RX_ADDR_P1 = 0x0B,
                RX_ADDR_P2 = 0x0C,
                RX_ADDR_P3 = 0x0D,
                RX_ADDR_P4 = 0x0E,
                RX_ADDR_P5 = 0x0F,
                TX_ADDR = 0x10,
                RX_PW_P0 = 0x11,
                RX_PW_P1 = 0x12,
                RX_PW_P2 = 0x13,
                RX_PW_P3 = 0x14,
                RX_PW_P4 = 0x15,
                RX_PW_P5 = 0x16,
                FIFO_STATUS = 0x17,
                DYNPD = 0x1C,
                FEATURE = 0x1D
        };

        enum Nrf24L01PCommands {
                R_REGISTER = 0x00, // last 4 bits will indicate reg. address
                W_REGISTER = 0x20, // last 4 bits will indicate reg. address
                REGISTER_MASK = 0x1F,
                R_RX_PAYLOAD = 0x61,
                W_TX_PAYLOAD = 0xA0,
                FLUSH_TX = 0xE1,
                FLUSH_RX = 0xE2,
                REUSE_TX_PL = 0xE3,
                ACTIVATE = 0x50,
                R_RX_PL_WID = 0x60,
                NOP = 0xFF
        };

private:
        Spi *spi;
        Gpio *cePin;
        Gpio *irqPin;
        uint8_t configRegisterCopy = 0x08;
        std::function<void(void)> onData;
};

#endif // NRF24L0P_H
