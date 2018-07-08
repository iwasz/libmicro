/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "SymaX5HWRxProtocol.h"
#include "Debug.h"
#define DEBUG 1

/*****************************************************************************/

// const uint8_t SymaX5HWRxProtocol::START_CHANS_1[] = { 0x0a, 0x1a, 0x2a, 0x3a };
// const uint8_t SymaX5HWRxProtocol::START_CHANS_2[] = { 0x2a, 0x0a, 0x42, 0x22 };
// const uint8_t SymaX5HWRxProtocol::START_CHANS_3[] = { 0x1a, 0x3a, 0x12, 0x32 };

const uint8_t SymaX5HWRxProtocol::BIND_CHANNELS[] = { 9, 137 };
const uint8_t SymaX5HWRxProtocol::RX_CHANNELS[] = { 53, 59, 65, 71, 181, 187, 193, 199 };
const uint8_t SymaX5HWRxProtocol::BIND_ADDR[] = { 0xab, 0xac, 0xad, 0xae, 0xaf };

/*****************************************************************************/

SymaX5HWRxProtocol::SymaX5HWRxProtocol (Nrf24L01P *n) : nrf (n), state (BINDING), mRfChNum (0), packetNo (0)
{
        nrf->setConfig (Nrf24L01P::MASK_NO_IRQ, true, Nrf24L01P::CRC_LEN_2);
        nrf->setTxAddress (SymaX5HWRxProtocol::BIND_ADDR, 5);
        nrf->setRxAddress (0, SymaX5HWRxProtocol::BIND_ADDR, 5);
        nrf->setAutoAck (0);
        nrf->setEnableDataPipe (Nrf24L01P::ERX_P0);
        nrf->setAdressWidth (Nrf24L01P::WIDTH_5);
        nrf->setChannel (SymaX5HWRxProtocol::BIND_CHANNELS[0]);
        nrf->setAutoRetransmit (Nrf24L01P::WAIT_4000_US, Nrf24L01P::RETRANSMIT_15);
        nrf->setPayloadLength (0, SymaX5HWRxProtocol::RX_PACKET_SIZE);
        nrf->setDataRate (Nrf24L01P::KBPS_250, Nrf24L01P::DBM_0);
        nrf->powerUp (Nrf24L01P::RX);
        nrf->setCallback (this);
}

/*****************************************************************************/

uint8_t SymaX5HWRxProtocol::checksum (uint8_t *data) const
{
        uint8_t sum = data[0];

        for (int i = 1; i < RX_PACKET_SIZE - 1; i++) {
                sum ^= data[i];
        }

        return sum + 0x55;
}

/*****************************************************************************/

void SymaX5HWRxProtocol::onRx (uint8_t *data, size_t len) { onPacket (data); }

/*****************************************************************************/

void SymaX5HWRxProtocol::onPacket (uint8_t *packet)
{
        switch (state) {
        case BINDING:
                onBindPacket (packet);
                break;

        case RECEIVING:
                onReceivePacket (packet);
                break;

        default:
                break;
        }
}

/*****************************************************************************/

void SymaX5HWRxProtocol::onBindPacket (uint8_t *packet)
{
        if (packet[5] != 0xaa || packet[6] != 0xaa || packet[RX_PACKET_SIZE - 1] != checksum (packet)) {
                return;
        }
#ifdef DEBUG
        Debug *d = Debug::singleton ();
        d->print ("BIND : ");
        d->printArray (packet, 10);
        d->print ("\n");
#endif
        uint8_t addr[5];

        for (int k = 0; k < 5; ++k) {
                addr[k] = packet[4 - k];
        }

#ifdef DEBUG
        d->print ("New address : ");
        d->printArray (addr, 5);
        d->print ("\n");
#endif
        nrf->setRxAddress (0, addr, 5);
        // setRFChannels (addr[0]);

        mRfChNum = 0;
        // nrf->setChannel (mRFChanBufs[mRfChNum]);
        nrf->setChannel (RX_CHANNELS[mRfChNum]);
#ifdef DEBUG
        d->print ("Channels : ");
        d->printArray (mRFChanBufs, FREQ_HOPS_SIZE);
        d->print ("\n");
#endif
        state = RECEIVING;
        nrf->flushRx ();
}

/*****************************************************************************/

/*
 * Magic method from the Internets.
 * Channels determined by last byte of tx address.
 * Note : it does not work for X5HW
 */
// void SymaX5HWRxProtocol::setRFChannels (uint8_t address)
//{
//        uint8_t laddress = address & 0x1f;
//        uint8_t i;
//        uint32_t *pchans = (uint32_t *)mRFChanBufs; // avoid compiler warning

//        if (laddress < 0x10) {
//                if (laddress == 6) {
//                        laddress = 7;
//                }

//                for (i = 0; i < FREQ_HOPS_SIZE; i++) {
//                        mRFChanBufs[i] = START_CHANS_1[i] + laddress;
//                }
//        }
//        else if (laddress < 0x18) {

//                for (i = 0; i < FREQ_HOPS_SIZE; i++) {
//                        mRFChanBufs[i] = START_CHANS_2[i] + (laddress & 0x07);
//                }

//                if (laddress == 0x16) {
//                        mRFChanBufs[0] += 1;
//                        mRFChanBufs[1] += 1;
//                }
//        }
//        else if (laddress < 0x1e) {
//                for (i = 0; i < FREQ_HOPS_SIZE; i++) {
//                        mRFChanBufs[i] = START_CHANS_3[i] + (laddress & 0x07);
//                }
//        }
//        else if (laddress == 0x1e) {
//                *pchans = 0x38184121;
//        }
//        else {
//                *pchans = 0x39194121;
//        }
//}

/*****************************************************************************/

void SymaX5HWRxProtocol::onReceivePacket (uint8_t *packet)
{
        RxValues rxValue;
        rxValue.throttle = packet[0];

        rxValue.yaw = packet[2];
        if (rxValue.yaw < 0) {
                rxValue.yaw = 128 - rxValue.yaw;
        }

        rxValue.pitch = packet[1];
        if (rxValue.pitch < 0) {
                rxValue.pitch = 128 - rxValue.pitch;
        }

        rxValue.roll = packet[3];
        if (rxValue.roll < 0) {
                rxValue.roll = 128 - rxValue.roll;
        }

        rxValue.trimYaw = packet[6] & 0x3F;
        if (rxValue.trimYaw >= 32) {
                rxValue.trimYaw = 32 - rxValue.trimYaw;
        }

        rxValue.trimPitch = packet[5] & 0x3F;
        if (rxValue.trimPitch >= 32) {
                rxValue.trimPitch = 32 - rxValue.trimPitch;
        }

        rxValue.trimRoll = packet[7] & 0x3F;
        if (rxValue.trimRoll >= 32) {
                rxValue.trimRoll = 32 - rxValue.trimRoll;
        }

        rxValue.trimThrottle = packet[4] & 0x3F;
        if (rxValue.trimThrottle >= 32) {
                rxValue.trimThrottle = 32 - rxValue.trimThrottle;
        }

        rxValue.video = packet[4] & 0x80;
        rxValue.picture = packet[4] & 0x40;
        rxValue.highspeed = packet[5] & 0x80;
        rxValue.flip = packet[6] & 0x40;

#ifdef DEBUG
        Debug *d = Debug::singleton ();
        d->printArray (packet, 10);
        d->print ("  ");
        d->print (rxValue.trimYaw);
        d->print ("  ");
        d->print (rxValue.trimPitch);
        d->print ("  ");
        d->print (rxValue.trimRoll);
        d->print ("  ");
        d->print (rxValue.trimThrottle);
        d->print ("\n");
#endif

        if (onRxValues) {
                onRxValues (rxValue);
        }

        if (++packetNo % 2 == 0) {
                mRfChNum = (mRfChNum + 1) % FREQ_HOPS_SIZE;
                nrf->setChannel (RX_CHANNELS[mRfChNum]);
        }
}
