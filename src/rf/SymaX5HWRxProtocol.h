/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef SYMAX5HWRXPROTOCOL_H
#define SYMAX5HWRXPROTOCOL_H

#include "Nrf24L01P.h"
#include <functional>

/**
 * Syma X5HW protocol. But unfortunately it seems, that only my particular
 * transmitter will work with this code, because I was unable to confirm how
 * X5HW hops the frequencies. It does it differently from regular X5. Iwas
 * unable to snif for SPI transmission, because it uses Beken bk2461 chip,
 * which is a SoC, which integrates MCS51 µC alongside with nRF compatible
 * transceiverm, so SPI is not exposed. But swiping on all the canals I was
 * able to observe valid ones, and I can develop an algorithm basing only on
 * one transmitter.
 *
 * Code based on:
 * https://github.com/Suxsem/symaxrx
 * https://github.com/DeviationTX/deviation
 */
class SymaX5HWRxProtocol : public Nrf24L01PCallback {
public:
        struct RxValues {
                uint8_t throttle;    // 0...255
                int8_t yaw;          // 127...-127
                int8_t pitch;        // 127...-127
                int8_t roll;         // 127...-127
                int8_t trimYaw;      // 31...-31
                int8_t trimPitch;    // 31...-31
                int8_t trimRoll;     // 31...-31
                int8_t trimThrottle; // 31...-31
                bool video;
                bool picture;
                bool highspeed;
                bool flip;
        };

        SymaX5HWRxProtocol (Nrf24L01P *n) : nrf (n), state (BINDING), mRfChNum (0), packetNo (0) {}
        virtual ~SymaX5HWRxProtocol () {}

        void onPacket (uint8_t *packet);
        virtual void onRx (uint8_t *data, size_t len) { onPacket (data); }
        virtual void onTx () {}
        virtual void onMaxRt () {}
        void run ();

        static constexpr int RX_PACKET_SIZE = 10;
        static constexpr int FREQ_HOPS_SIZE = 8;
        static constexpr int CHANNEL_DELAY_MS = 128;

        static const uint8_t BIND_ADDR[];
        static const uint8_t BIND_CHANNELS[];
        static const uint8_t RX_CHANNELS[];

        std::function<void(RxValues const &)> onRxValues;

private:
        enum State { BINDING, RECEIVING };

        void onBindPacket (uint8_t *packet);
        void onReceivePacket (uint8_t *packet);

        // static const uint8_t START_CHANS_1[];
        // static const uint8_t START_CHANS_2[];
        // static const uint8_t START_CHANS_3[];

        uint8_t checksum (uint8_t *data) const;
        // void setRFChannels (uint8_t address);

private:
        Nrf24L01P *nrf;
        State state;
        uint8_t mRFChanBufs[FREQ_HOPS_SIZE];
        int mRfChNum;
        int packetNo;
        RxValues values[2];
};

#endif // SYMAX5HWRXPROTOCOL_H
