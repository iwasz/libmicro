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

        explicit LineSink (QueueT &g, CanLooseData canLooseData = CanLooseData::NO) : receivedDataQueue (g), canLooseData (canLooseData) {}
        LineSink (LineSink const &) = delete;
        LineSink &operator= (LineSink const &) = delete;
        LineSink (LineSink &&) = delete;
        LineSink &operator= (LineSink &&) = delete;
        ~LineSink () override = default;

        void onData (uint8_t c) override;
        void onError (uint32_t /* error */) override { Error_Handler (); }

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
        LineT line; /// Bufor wejściowy na odpowiedzi z modemu. Mamy własny, gdyż kolejka może się w między czasie wyczyścić.
        QueueT &receivedDataQueue;
        CanLooseData canLooseData;
};

/*****************************************************************************/

template <typename QueueT, typename LineT> void LineSink<QueueT, LineT>::onData (uint8_t c)
{
#ifdef ALL_DATA_DEBUG
        addAllData (c);
#endif

        if (!bool(std::isprint (c)) && c != '\r' && c != '\n') {
                return;
        }

        if (!line.empty () && (c == '\r' || c == '\n')) {
                gsl::final_action clearPos{ [this] { line.clear (); } };

                // Nie wrzucamy na kolejkę odpowiedzi, które zawierają tylko \r\n
                if (line.size () > 1) {
                        if (receivedDataQueue.full ()) {
                                if (canLooseData == CanLooseData::YES) {
                                        return;
                                }

                                Error_Handler ();
                        }

                        receivedDataQueue.push_back (std::move (line));
                }
        }
        else if (bool(std::isprint (c))) {
                line.push_back (c);
        }
}
