/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef __DEBUG_DRIVER_H__
#define __DEBUG_DRIVER_H__

#include <Hal.h>
#include <cstdint>
#include <cstdlib>

class Usart;

class Debug {
public:
        static Debug *&singleton ();

        Debug (Usart *uart) : uart (uart) {}

        void print (const char *str);
        void println (const char *str);
        void print (uint8_t *data, size_t len);
        void printArray (uint8_t *data, size_t len);
        void print (int);
        void println (int);
        // void print (unsigned int);
        // void println (unsigned int);

        void printTime (uint16_t time);

public:
        Usart *uart;
        static Debug *instance;
};

#endif //__CMD_UART_H__
