/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "LineSink.h"
#include <algorithm>

/**
 * In normal circumstances it works exactly the same as its base class LineSink.
 * But when receiveBytes is used it switches to "fixed" mode where it waits for
 * exact number of bytes that was set using receiveBytes method. Those bytes can
 * be non-printable. After receiving this fixed amout it switches back to normal
 * "lineSink" mode.
 */
template <typename QueueT, typename LineT> class FixedLineSink : public LineSink<QueueT, LineT> {
public:
        using QueueType = QueueT;

        explicit FixedLineSink (QueueT &q) : LineSink<QueueType, LineT> (q) {}
        FixedLineSink (FixedLineSink const &) = delete;
        FixedLineSink &operator= (FixedLineSink const &) = delete;
        FixedLineSink (FixedLineSink &&) = delete;
        FixedLineSink &operator= (FixedLineSink &&) = delete;
        ~FixedLineSink () override = default;

        void onData (uint8_t c) override;

        /**
         * Set this to something biger than 0 for fixed number reception. If set, this
         * sink will receive fixed number of bytes. After reception it resets to normal
         * operaration.
         */
        void receiveBytes (size_t b);

private:
        size_t fixedNumberOfBytes = 0;
        LineT fixedLine;
};

/*****************************************************************************/

template <typename QueueT, typename LineT> void FixedLineSink<QueueT, LineT>::onData (uint8_t c)
{
        if (fixedNumberOfBytes > 0) {
#ifdef ALL_DATA_DEBUG
                LineSink<QueueType, LineT>::addAllData ('#');
                LineSink<QueueType, LineT>::addAllData (c);
#endif

                fixedLine.push_back (c);

                if (fixedLine.size () == fixedNumberOfBytes) {
                        auto &lineQueue = LineSink<QueueT, LineT>::receivedDataQueue;

                        if (lineQueue.full ()) {
                                Error_Handler ();
                        }

                        lineQueue.push_back (std::move (fixedLine));
                        fixedNumberOfBytes = 0;
                        fixedLine.clear ();
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
        fixedLine.clear ();
}

