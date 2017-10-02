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

class Debug {
public:
        static Debug *singleton ();
        void init (uint32_t speed);

        void print (const char *str);
        void print (uint8_t *data, size_t len);
        void print (int);
        void printTime (uint16_t time);

private:
        UART_HandleTypeDef huart;
};

#endif //__CMD_UART_H__
