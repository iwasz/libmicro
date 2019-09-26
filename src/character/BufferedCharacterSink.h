/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Debug.h"
#include "ErrorHandler.h"
#include "Hal.h"
#include "ICharacterSink.h"
#include <cstdlib>
#include <etl/queue_spsc_isr.h>

template <size_t MAX_SIZE, bool FATAL_IF_FULL = true> class BufferedCharacterSink : public ICharacterSink {
public:
        using QueueType = etl::queue_spsc_isr<uint8_t, MAX_SIZE, CortexMInterruptControl>;

        BufferedCharacterSink (ICharacterSink &child) : childSink (child) {}
        BufferedCharacterSink (BufferedCharacterSink const &) = delete;
        BufferedCharacterSink &operator= (BufferedCharacterSink const &) = delete;
        BufferedCharacterSink (BufferedCharacterSink &&) = delete;
        BufferedCharacterSink &operator= (BufferedCharacterSink &&) = delete;
        ~BufferedCharacterSink () override = default;

        /// Called from ISR
        void onData (uint8_t c) override;

        /// Called from ISR
        void onError (uint32_t /*error*/) override { Error_Handler (); }

        /// Call periodicaly from a main loop (not from ISR).
        void run ();

//#define ALL_DATA_DEBUG_BUFFERED
#ifdef ALL_DATA_DEBUG_BUFFERED
        static constexpr size_t ALL_DATA_MAX_SIZE = 2048;
        uint8_t allData[ALL_DATA_MAX_SIZE];
        size_t allDataCnt = 0;

        void addAllData (uint8_t c)
        {
                if (allDataCnt < ALL_DATA_MAX_SIZE) {
                        allData[allDataCnt++] = c;
                }
        }
#endif

private:
        QueueType queue;
        ICharacterSink &childSink;
        bool dataLostMarker = false;
};

/*****************************************************************************/

template <size_t MAX_SIZE, bool FATAL_IF_FULL> inline void BufferedCharacterSink<MAX_SIZE, FATAL_IF_FULL>::onData (uint8_t c)
{
#ifdef ALL_DATA_DEBUG_BUFFERED
        addAllData (c);
#endif

        if (!queue.push_from_isr (c)) {

                if constexpr (FATAL_IF_FULL) {
                        // Too much data at once.
                        Error_Handler ();
                }
                else {
                        dataLostMarker = true;
                }
        }
}

/*****************************************************************************/

template <size_t MAX_SIZE, bool FATAL_IF_FULL> void BufferedCharacterSink<MAX_SIZE, FATAL_IF_FULL>::run ()
{
        if (!queue.empty ()) {
                uint8_t c;
                queue.pop (c);
                childSink.onData (c);
        }
}
