/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#include "I2c.h"
#include <cstring>

/* Read/Write command */
#define READWRITE_CMD ((uint8_t)0x80)
/* Multiple byte read/write command */
#define MULTIPLEBYTE_CMD ((uint8_t)0x40)

/*****************************************************************************/

I2c::I2c ()
{
        memset (&i2cHandle, 0, sizeof (i2cHandle));

        // TODO move to some method
        __HAL_RCC_I2C1_CLK_ENABLE ();

        i2cHandle.Instance = I2C1;
#ifdef LIB_MICRO_STM32F4
        i2cHandle.Init.ClockSpeed = 400000;
        i2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
#else
        i2cHandle.Init.Timing = 0x2000090E; // 100kHz
#endif
        i2cHandle.Init.OwnAddress1 = 0x3f << 1;
        i2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        i2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        i2cHandle.Init.OwnAddress2 = 0;
        i2cHandle.Init.OwnAddress2Masks = 0;
        i2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        i2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

        if (HAL_I2C_Init (&i2cHandle) != HAL_OK) {
                Error_Handler ();
        }

        HAL_I2CEx_ConfigAnalogFilter (&i2cHandle, I2C_ANALOGFILTER_ENABLE);
}

/*****************************************************************************/

void I2c::read (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout)
{
        //        if (length > 1) {
        //                regAddr |= (uint8_t) (READWRITE_CMD | MULTIPLEBYTE_CMD);
        //        }
        //        else {
        //                regAddr |= (uint8_t)READWRITE_CMD;
        //        }

        //        while (HAL_I2C_Master_Sequential_Transmit_IT (&I2cHandle, (uint16_t)devAddr, &regAddr, 1, I2C_FIRST_FRAME) != HAL_OK) {
        while (HAL_I2C_Master_Transmit (&i2cHandle, (uint16_t)devAddr << 1, &regAddr, 1, timeout) != HAL_OK) {
                if (HAL_I2C_GetError (&i2cHandle) != HAL_I2C_ERROR_AF) {
                        Error_Handler ();
                }
        }

        while (HAL_I2C_GetState (&i2cHandle) != HAL_I2C_STATE_READY) {
        }

        //        while (HAL_I2C_Master_Sequential_Receive_IT (&I2cHandle, (uint16_t)devAddr, data, length, I2C_LAST_FRAME) != HAL_OK) {
        while (HAL_I2C_Master_Receive (&i2cHandle, (uint16_t)devAddr << 1, data, length, timeout) != HAL_OK) {
                if (HAL_I2C_GetError (&i2cHandle) != HAL_I2C_ERROR_AF) {
                        Error_Handler ();
                }
        }

        while (HAL_I2C_GetState (&i2cHandle) != HAL_I2C_STATE_READY) {
        }
}

/*****************************************************************************/

void I2c::write (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout)
{
        uint8_t tmp[128];
        tmp[0] = regAddr;

        if (length <= 127) {
                memcpy (tmp + 1, data, length);
        }
        else {
                Error_Handler ();
        }

        while (HAL_I2C_Master_Transmit (&i2cHandle, (uint16_t)devAddr << 1, tmp, length + 1, timeout) != HAL_OK) {
                if (HAL_I2C_GetError (&i2cHandle) != HAL_I2C_ERROR_AF) {
                        Error_Handler ();
                }
        }
}

void I2c::slaveRead (uint8_t *data, size_t length, uint16_t timeout)
{
        if (HAL_I2C_Slave_Receive (&i2cHandle, data, length, timeout) != HAL_OK) {
                Error_Handler ();
        }
}

void I2c::slaveWrite (uint8_t *data, size_t length, uint16_t timeout)
{
        if (HAL_I2C_Slave_Transmit (&i2cHandle, data, length, timeout) != HAL_OK) {
                Error_Handler ();
        }
}
