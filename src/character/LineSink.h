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
 * lines. Every line is then placed on a Queue.
 *
 * TODO this API is broken, or at least the names are. For instance why EventT? It should be
 * named LineT or StringT something. This way as it is now, it creates some "events" not the
 * promised strings of text.
 */
template <typename QueueT, typename LineT> class LineSink : public ICharacterSink {
public:
        enum class CanLooseData { NO, YES };

        LineSink (QueueT &g, CanLooseData canLooseData = CanLooseData::NO) : receivedDataQueue (g), canLooseData (canLooseData) {}
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
        /// Bufor wejściowy na odpowiedzi z modemu. Mamy własny, gdyż kolejka może się w między czasie wyczyścić.
        std::array<uint8_t, 160> rxBufferGsm{};
        QueueT &receivedDataQueue;
        CanLooseData canLooseData;
};

/*****************************************************************************/

template <typename QueueT, typename LineT> void LineSink<QueueT, LineT>::onData (uint8_t c)
{
#ifdef ALL_DATA_DEBUG
        addAllData (c);
#endif

        if (!std::isprint (c) && c != '\r' && c != '\n') {
                return;
        }

        if (rxBufferGsmPos > 0 && (c == '\r' || c == '\n')) {
                gsl::final_action clearPos{ [this] { rxBufferGsmPos = 0; } };

                if (rxBufferGsmPos >= rxBufferGsm.max_size ()) {
                        // debug->println ("L. ovf");
                        // return;
                        Error_Handler ();
                }

                rxBufferGsm[rxBufferGsmPos] = '\0';

                // Nie wrzucamy na kolejkę odpowiedzi, które zawierają tylko \r\n
                if (rxBufferGsmPos > 1) {
#if 0
                        debug->print ("rx : ");
                        debug->println (rxBufferGsm, rxBufferGsmPos);
#endif

                        if (receivedDataQueue.full ()) {
                                if (canLooseData == CanLooseData::YES) {
                                        return;
                                }

                                Error_Handler ();
                        }

                        // auto &queueBuffer = receivedDataQueue.back ();

                        // TODO Optimize - for example etl::queue has an emplace method.
                        // TODO kolejka powinna przyjmować stringi o zmiennej długości.
                        if (rxBufferGsmPos > LineT::MAX_SIZE) {
                                Error_Handler ();
                        }

                        // TODO czemu to muszę czyścić? Jeśli tego nie zrobię, to czaem się sklejają linijki.
                        // TODO z tym poniżej działa niestabilnie i nadal sklejają się czasem linijki! WTF!
                        // queueBuffer.clear ();
                        // std::copy_n (rxBufferGsm.cbegin (), rxBufferGsmPos, std::back_inserter (queueBuffer));
                        receivedDataQueue.push_back (LineT{ rxBufferGsm.data (), rxBufferGsm.data () + rxBufferGsmPos });
                }
        }
        else if (std::isprint (c)) {
                if (rxBufferGsmPos >= rxBufferGsm.max_size ()) {
                        // debug->println ("L. ovf");
                        // return;
                        Error_Handler ();
                }

                rxBufferGsm[rxBufferGsmPos++] = c;
        }
}
