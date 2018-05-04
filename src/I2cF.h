/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_I2C_H
#define LIB_MICRO_I2C_H

#include "Hal.h"
#include <cstdint>
#include <cstdlib>

extern "C" void I2C1_IRQHandler ();

class I2c {
public:
        I2c ();

        enum { DEFAULT_TIMEOUT = 1000 };
        void read (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);
        void write (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);

        void slaveRead (uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);
        void slaveWrite (uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);

        void slaveReadIt (uint8_t *data, size_t length);

private:
        friend void I2C1_IRQHandler ();
        I2C_HandleTypeDef i2cHandle;
        static I2c *i2c1;
        static I2c *i2c2;
};

#endif // I2C_H
