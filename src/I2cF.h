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
#include "II2cCallback.h"
#include <cstdint>
#include <cstdlib>

extern "C" void I2C1_IRQHandler ();
extern "C" void HAL_I2C_SlaveRxCpltCallback (I2C_HandleTypeDef *I2cHandle);

/**
 * @brief The I2c class
 */
class I2c {
public:
        I2c ();

        enum { DEFAULT_TIMEOUT = 1000 };
        void read (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);
        void write (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);

        void slaveRead (uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);
        void slaveWrite (uint8_t *data, size_t length, uint16_t timeout = DEFAULT_TIMEOUT);

        void slaveReadIt (uint8_t *data, size_t length);

        void setCallback (II2cCallback *c) { callback = c; }
        void waitStatus ();
        void waitStatusReady ();
        void listen ();

private:
        friend void I2C1_IRQHandler ();
        friend void HAL_I2C_SlaveRxCpltCallback (I2C_HandleTypeDef *I2cHandle);
        I2C_HandleTypeDef i2cHandle;
        II2cCallback *callback;
        static I2c *i2c1;
        static I2c *i2c2;
};

#endif // I2C_H
