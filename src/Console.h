/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_CONSOLE_H
#define LIB_MICRO_CONSOLE_H

#include "Usart.h"
#include <cstdint>

class Console {
public:
        enum { MAX_RX_BUFFER = 128 };

        Console (Usart *u);
        virtual ~Console () { delete[] rxBuffer; }

        /**
         * @brief Can be called by USART ISR or something similar.
         * @param c
         */
        void onData (uint8_t c);

        /**
         * @brief To be called in the main loop.
         */
        void run ();

        virtual void onNewLine (const char *str, size_t len);

private:
        Usart *usart;
        int rxBufferPos;
        char *rxBuffer;
};

#endif // CONSOLE_H
