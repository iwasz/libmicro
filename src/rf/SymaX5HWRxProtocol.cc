/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "SymaX5HWRxProtocol.h"
#include "Debug.h"
#include <cstring>
#define DEBUG 1

/*****************************************************************************/

const uint8_t bindPacket[] = { 0xf9, 0x96, 0x82, 0x1b, 0x20, 0x08, 0x08, 0xf2, 0x7d, 0xef };

/*****************************************************************************/

SymaX5HWRxProtocol::SymaX5HWRxProtocol (Nrf24L01P *n) : nrf (n), state (INIT), currentChannel (0), packetNo (0)
{
        buildStateInfo ();
        nrf->setConfig (Nrf24L01P::MASK_NO_IRQ, true, Nrf24L01P::CRC_LEN_2);
        nrf->setAutoAck (0);
        nrf->setEnableDataPipe (Nrf24L01P::ERX_P0);
        nrf->setAdressWidth (Nrf24L01P::WIDTH_5);
        setStateAddress (INIT);
        setStateChannels (INIT);
        setStatePacketLength (INIT);
        nrf->setAutoRetransmit (Nrf24L01P::WAIT_4000_US, Nrf24L01P::RETRANSMIT_15);
        nrf->setDataRate (Nrf24L01P::MBPS_1, Nrf24L01P::DBM_0);
        nrf->powerUp (Nrf24L01P::RX);
        nrf->setCallback (this);
        state = BINDING;
}

/*****************************************************************************/

void SymaX5HWRxProtocol::buildStateInfo ()
{
        static uint8_t initChannels[] = { 0x08 };
        static uint8_t initAddress[] = { 0x6d, 0x6a, 0x73, 0x73, 0x73 };
        static StateInfo init{ initChannels, initAddress, sizeof (initChannels), sizeof (initAddress), 15 };
        stateInfo[INIT] = &init;

        static uint8_t bindChannels[] = { 0x27, 0x1b, 0x39, 0x28, 0x24, 0x22, 0x2e, 0x36, 0x19, 0x21, 0x29, 0x14, 0x1e, 0x12, 0x2d, 0x18 };
        static StateInfo bind{ bindChannels, nullptr, sizeof (bindChannels), 0, 16 };
        stateInfo[BINDING] = &bind;

        static uint8_t receiveChannels[] = { 0x1d, 0x2f, 0x26, 0x3d, 0x15, 0x2b, 0x25, 0x24, 0x27, 0x2c, 0x1c, 0x3e, 0x39, 0x2d, 0x22 };
        static StateInfo receive{ receiveChannels, nullptr, sizeof (receiveChannels), 0, 16 };
        stateInfo[RECEIVING] = &receive;
}

/*****************************************************************************/

void SymaX5HWRxProtocol::setStateChannels (State state, uint8_t *channels, uint8_t channelsLength)
{
        this->currentChannel = 0;

        if (channels) {
                this->channels = channels;
                this->channelsNum = channelsLength;
        }
        else {
                StateInfo *info = stateInfo[state];
                this->channels = info->channels;
                this->channelsNum = info->channelsLength;
        }

        nrf->setChannel (this->channels[this->currentChannel]);
}

/*****************************************************************************/

void SymaX5HWRxProtocol::setStateAddress (State state, uint8_t *address, uint8_t addressLength)
{
        if (address) {
                this->address = address;
                this->addressLength = addressLength;
        }
        else {
                StateInfo *info = stateInfo[state];

                if (info->address && info->addressLength) {
                        this->address = info->address;
                        this->addressLength = info->addressLength;
                }
        }

        nrf->setTxAddress (this->address, 5);
        nrf->setRxAddress (0, this->address, 5);
}

/*****************************************************************************/

void SymaX5HWRxProtocol::setStatePacketLength (State state)
{
        StateInfo *info = stateInfo[state];
        this->packetLength = info->packetLength;
        nrf->setPayloadLength (0, this->packetLength);
}

/*****************************************************************************/

uint8_t SymaX5HWRxProtocol::checksum (uint8_t *data, size_t packetSize) const
{
        uint8_t sum = data[0];

        for (int i = 1; i < packetSize - 1; i++) {
                sum ^= data[i];
        }

        return sum + 0x55;
}

/*****************************************************************************/

void SymaX5HWRxProtocol::onRx (uint8_t *data, size_t len) { onPacket (data); }

/*****************************************************************************/

void SymaX5HWRxProtocol::onPacket (uint8_t *packet)
{
        if (packetNo++ % 2) { // use each channel twice
                currentChannel = (currentChannel + 1) % channelsNum;
        }

        switch (state) {
        case BINDING:
                onBindPacket (packet);
                break;

        case BINDING2:
                onBind2Packet (packet);
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
#ifdef DEBUG
        Debug *d = Debug::singleton ();
        d->print ("BIND : ");
        d->printArray (packet, 10);
        d->print ("\n");
#endif

        if (memcmp (bindPacket, packet, sizeof (bindPacket)) != 0) {
                return;
        }

#ifdef DEBUG
        d->print ("Packet OK\n");
#endif
        //        uint8_t addr[5];

        //        for (int k = 0; k < 5; ++k) {
        //                addr[k] = packet[4 - k];
        //        }

        //#ifdef DEBUG
        //        d->print ("New address : ");
        //        d->printArray (addr, 5);
        //        d->print ("\n");
        //#endif
        //        nrf->setRxAddress (0, addr, 5);
        // setRFChannels (addr[0]);
        // nrf->setChannel (mRFChanBufs[mRfChNum]);
        //        nrf->setChannel (RX_CHANNELS[mRfChNum]);
        setStateChannels (BINDING);
        setStatePacketLength (BINDING);

        state = BINDING2;
        nrf->flushRx ();
}

/*****************************************************************************/

void SymaX5HWRxProtocol::onBind2Packet (uint8_t *packet)
{
#ifdef DEBUG
        Debug *d = Debug::singleton ();
        d->print ("BIND2 : ");
        d->printArray (packet, 16);
        d->print ("\n");
#endif

        if (packet[7] != 0xae || packet[8] != 0xa9 || packet[14] != 0xc0 || packet[15] != 0x17) {
                return;
        }

        setStateChannels (RECEIVING);
        setStatePacketLength (RECEIVING);
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

        //        if (++packetNo % 2 == 0) {
        //                mRfChNum = (mRfChNum + 1) % FREQ_HOPS_SIZE;
        //                nrf->setChannel (RX_CHANNELS[mRfChNum]);
        //        }
}
