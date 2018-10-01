/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef BLINKER_H
#define BLINKER_H

#include "Gpio.h"
#include "Timer.h"
#include <cstdarg>
#include <cstdint>
#include <cstdlib>

/**
 * Blinking pattern. Initially it is running.
 */
template <size_t SLOTS_NO> class Blinker {
public:
        /**
         * Variadic constructor taking SLOTS_NO integer arguments.
         * First argument is the time (in ms) during the Gpio is turned off.
         * Second argument is the time dusring the Gpio is turned on.
         * And so on.
         */
        Blinker (Gpio *g, int i, ...);

        /// Starts
        void start ();
        void stop () { running = false; }
        void run ();

public:
        bool running = true;
        uint8_t curentSlotNo = 0;
        uint16_t timeSlots[SLOTS_NO];
        Timer timer;
        Gpio *gpio;
};

/*****************************************************************************/

template <size_t SLOTS_NO> Blinker<SLOTS_NO>::Blinker (Gpio *g, int slots, ...) : gpio (g)
{
        va_list args;
        va_start (args, slots);
        timeSlots[0] = slots;

        for (int i = 1; i < SLOTS_NO; ++i) {
                timeSlots[i] = va_arg (args, int);
        }

        va_end (args);
}

/*****************************************************************************/

template <size_t SLOTS_NO> void Blinker<SLOTS_NO>::start ()
{
        running = true;
        timer.start (timeSlots[curentSlotNo++]);
        curentSlotNo %= SLOTS_NO;
}

/*****************************************************************************/

template <size_t SLOTS_NO> void Blinker<SLOTS_NO>::run ()
{
        if (running && timer.isExpired ()) {
                // Even and 0 means led turned off.
                gpio->set (curentSlotNo % 2 != 0);
                start ();
        }
}

#endif // BLINKER_H
