/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "SymaX5HWRxProtocol.h"
#include "Debug.h"

/*****************************************************************************/

// const uint8_t SymaX5HWRxProtocol::START_CHANS_1[] = { 0x0a, 0x1a, 0x2a, 0x3a };
// const uint8_t SymaX5HWRxProtocol::START_CHANS_2[] = { 0x2a, 0x0a, 0x42, 0x22 };
// const uint8_t SymaX5HWRxProtocol::START_CHANS_3[] = { 0x1a, 0x3a, 0x12, 0x32 };

const uint8_t SymaX5HWRxProtocol::BIND_CHANNELS[] = { 9, 137 };
const uint8_t SymaX5HWRxProtocol::RX_CHANNELS[] = { 53, 59, 65, 71, 181, 187, 193, 199 };
const uint8_t SymaX5HWRxProtocol::BIND_ADDR[] = { 0xab, 0xac, 0xad, 0xae, 0xaf };

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

        // Debug::singleton ()->print ("BIND : ");
        // Debug::singleton ()->printArray (packet, 10);
        // Debug::singleton ()->print ("\n");

        uint8_t addr[5];

        for (int k = 0; k < 5; ++k) {
                addr[k] = packet[4 - k];
        }

        // Debug::singleton ()->print ("New address : ");
        // Debug::singleton ()->printArray (addr, 5);
        // Debug::singleton ()->print ("\n");

        nrf->setRxAddress (0, addr, 5);
        // setRFChannels (addr[0]);

        mRfChNum = 0;
        // nrf->setChannel (mRFChanBufs[mRfChNum]);
        nrf->setChannel (RX_CHANNELS[mRfChNum]);

        // Debug::singleton ()->print ("Channels : ");
        // Debug::singleton ()->printArray (mRFChanBufs, FREQ_HOPS_SIZE);
        // Debug::singleton ()->print ("\n");

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

        //        Debug::singleton ()->printArray (packet, 10);
        //        Debug::singleton ()->print ("  ");
        //        Debug::singleton ()->print (rxValue.trimYaw);
        //        Debug::singleton ()->print ("  ");
        //        Debug::singleton ()->print (rxValue.trimPitch);
        //        Debug::singleton ()->print ("  ");
        //        Debug::singleton ()->print (rxValue.trimRoll);
        //        Debug::singleton ()->print ("  ");
        //        Debug::singleton ()->print (rxValue.trimThrottle);
        //        Debug::singleton ()->print ("\n");

        if (onRxValues) {
                onRxValues (rxValue);
        }

        if (++packetNo % 2 == 0) {
                mRfChNum = (mRfChNum + 1) % FREQ_HOPS_SIZE;
                nrf->setChannel (RX_CHANNELS[mRfChNum]);
        }
}
