/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef FIXEDLINESINK_H
#define FIXEDLINESINK_H

#include "LineSink.h"
#include <algorithm>

// TODO templatize BinaryEvent
template <typename QueueT, typename EventT> class FixedLineSink : public LineSink<QueueT, EventT> {
public:
        using QueueType = QueueT;

        FixedLineSink (QueueT &q) : LineSink<QueueType, EventT> (q) {}
        virtual ~FixedLineSink () = default;

        virtual void onData (uint8_t c) override;

        /**
         * Set this to something biger than 0 for fixed number reception. If set, this
         * sink will receive fixed number of bytes. After reception it resets to normal
         * operaration.
         */
        void receiveBytes (size_t b);

private:
        size_t fixedNumberOfBytes = 0;
        size_t currentByte = 0;

        // TODO quick fix, rethink
        uint8_t tmpBuffer[128];
};

/*****************************************************************************/

template <typename QueueT, typename EventT> void FixedLineSink<QueueT, EventT>::onData (uint8_t c)
{
        if (fixedNumberOfBytes > 0) {
#ifdef ALL_DATA_DEBUG
                LineSink<QueueType, BinaryEvent>::addAllData ('#');
                LineSink<QueueType, BinaryEvent>::addAllData (c);
#endif

                tmpBuffer[currentByte++] = c;

                if (currentByte == fixedNumberOfBytes) {
                        if (!LineSink<QueueT, EventT>::gsmQueue.push_back ()) {
                                Error_Handler ();
                        }

                        auto &queueBuffer = LineSink<QueueT, EventT>::gsmQueue.back ();
                        queueBuffer.resize (fixedNumberOfBytes);
                        // TODO potetntialy optimize
                        std::copy (tmpBuffer, tmpBuffer + fixedNumberOfBytes, queueBuffer.begin ());
                        fixedNumberOfBytes = 0;
                        currentByte = 0;
                }
        }
        else {
                LineSink<QueueType, EventT>::onData (c);
        }
}

/*****************************************************************************/

template <typename QueueT, typename EventT> void FixedLineSink<QueueT, EventT>::receiveBytes (size_t b)
{
        fixedNumberOfBytes = b;
        currentByte = 0;
}

#endif // FIXEDLINESINK_H
