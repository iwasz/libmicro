/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_I2C_H
#define LIB_MICRO_I2C_H

#include <cstdint>
#include <cstdlib>

class I2c {
public:
        I2c ();

        enum { DEFAULT_TIMEOUT = 1000 };
        size_t read (uint8_t devAddr, uint8_t *data, size_t len, uint16_t timeout = DEFAULT_TIMEOUT);
        size_t write (uint8_t devAddr, uint8_t *data, size_t len, uint16_t timeout = DEFAULT_TIMEOUT);
};

#endif // I2C_H
