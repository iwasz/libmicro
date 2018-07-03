/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_TIMER_H
#define LIB_MICRO_TIMER_H

#include <cstdint>

class Timer {
public:
        Timer ();
        void start (uint32_t intervalMs);
        bool isExpired () const;
        uint32_t elapsed () const;
        static void delay (uint32_t delayMs);
        static uint32_t getTick ();

protected:
        uint32_t startTime;
        uint32_t intervalMs;
};

#endif //__TIMER_H__
