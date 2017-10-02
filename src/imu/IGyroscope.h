/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_IGUROSCOPE_H
#define LIB_MICRO_IGUROSCOPE_H

#include <cstdint>

struct IGyroscope {

        struct Data {
                int16_t x;
                int16_t y;
                int16_t z;
        };

        virtual ~IGyroscope () {}
        virtual Data getData () const = 0;
};

#endif // IACCELEROMETER_H
