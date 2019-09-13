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
template <typename QueueT, typename LineT> class FixedLineSink : public LineSink<QueueT, LineT> {
public:
        using QueueType = QueueT;

        FixedLineSink (QueueT &q) : LineSink<QueueType, LineT> (q) {}
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

template <typename QueueT, typename LineT> void FixedLineSink<QueueT, LineT>::onData (uint8_t c)
{
        if (fixedNumberOfBytes > 0) {
#ifdef ALL_DATA_DEBUG
                LineSink<QueueType, LineT>::addAllData ('#');
                LineSink<QueueType, LineT>::addAllData (c);
#endif

                tmpBuffer[currentByte++] = c;

                if (currentByte == fixedNumberOfBytes) {
                        auto &lineQueue = LineSink<QueueT, LineT>::receivedDataQueue;

                        if (lineQueue.full ()) {
                                Error_Handler ();
                        }

                        // auto &queueBuffer = LineSink<QueueT, LineT>::receivedDataQueue.back ();
                        // queueBuffer.resize (fixedNumberOfBytes);
                        // // TODO potetntialy optimize
                        // std::copy (tmpBuffer, tmpBuffer + fixedNumberOfBytes, queueBuffer.begin ());

                        lineQueue.push_back (LineT{ tmpBuffer, tmpBuffer + fixedNumberOfBytes });
                        fixedNumberOfBytes = 0;
                        currentByte = 0;
                }
        }
        else {
                LineSink<QueueType, LineT>::onData (c);
        }
}

/*****************************************************************************/

template <typename QueueT, typename LineT> void FixedLineSink<QueueT, LineT>::receiveBytes (size_t b)
{
        fixedNumberOfBytes = b;
        currentByte = 0;
}

#endif // FIXEDLINESINK_H
