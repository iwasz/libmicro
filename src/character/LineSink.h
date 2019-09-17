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
#include "character/ICharacterSink.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <gsl/gsl>

/**
 * Observes an character source like Usart and groups characters received into
 * lines. Every line is then placed on a Queue. Assumes all bytes are chcracters,
 * discards non printable ones.
 */
template <typename QueueT, typename LineT> class LineSink : public ICharacterSink {
public:
        enum class CanLooseData { NO, YES };
        enum class LineEnd { CR_LF, CR, LF, EITHER };

        explicit LineSink (QueueT &g, LineEnd lineEnd = LineEnd::CR_LF, CanLooseData canLooseData = CanLooseData::NO)
            : receivedDataQueue (g), canLooseData (canLooseData), lineEnd (lineEnd)
        {
        }

        LineSink (LineSink const &) = delete;
        LineSink &operator= (LineSink const &) = delete;
        LineSink (LineSink &&) = delete;
        LineSink &operator= (LineSink &&) = delete;
        ~LineSink () override = default;

        void onData (uint8_t c) override;
        void onError (uint32_t /* error */) override { Error_Handler (); }

        void receiveBytes (size_t b) { fixedNumberOfBytes = b; }

//#define ALL_DATA_DEBUG
#ifdef ALL_DATA_DEBUG
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

protected:
        uint16_t rxBufferGsmPos = 0;
        LineT line;                /// Current line we are assembling.
        QueueT &receivedDataQueue; /// Queue we will place line onto when finished.
        CanLooseData canLooseData;
        LineEnd lineEnd;
        size_t fixedNumberOfBytes = 0; /// Alternative mode where we do not split on CRLFs but we split after N bytes has been received.
};

/*****************************************************************************/

template <typename QueueT, typename LineT> void LineSink<QueueT, LineT>::onData (uint8_t c)
{
#ifdef ALL_DATA_DEBUG
        addAllData (c);
#endif

        line.push_back (c); // Will fail dramaticallyt if no memory left

        if (fixedNumberOfBytes > 0) { // Case 1 : waiting for specified numebr of bytes.
#ifdef ALL_DATA_DEBUG
                LineSink<QueueType, LineT>::addAllData ('#');
                LineSink<QueueType, LineT>::addAllData (c);
#endif

                if (line.size () == fixedNumberOfBytes) {
                        if (receivedDataQueue.full ()) {
                                if (canLooseData == CanLooseData::YES) {
                                        return;
                                }

                                Error_Handler ();
                        }

                        receivedDataQueue.push_back (std::move (line));
                        fixedNumberOfBytes = 0;
                        line.clear ();
                }
        }
        /* clang-format off */
        else if ((lineEnd == LineEnd::CR_LF && line.size () >= 2 && c == '\n' && line.at (line.size () - 2) == '\r') ||
                 (((lineEnd == LineEnd::EITHER && (c == '\r' || c == '\n')) ||
                  (lineEnd == LineEnd::LF && c == '\n') ||
                  (lineEnd == LineEnd::CR && c == '\r')) && !line.empty ()))
        /* clang-format on */
        {
                gsl::final_action clearPos{ [this] { line.clear (); } };

                if (receivedDataQueue.full ()) {
                        if (canLooseData == CanLooseData::YES) {
                                return;
                        }

                        Error_Handler ();
                }

                receivedDataQueue.push_back (std::move (line));
        }
}
