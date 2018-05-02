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

I2c::I2c (I2C_Type *instance, uint32_t clockSpeed) : i2c (instance)
{
        I2C_InitType i2cInitStruct;

        /* Enable I2C clock */
        if (instance == I2C2) {
                SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_I2C2, ENABLE);
        }
        else {
                SysCtrl_PeripheralClockCmd (CLOCK_PERIPH_I2C1, ENABLE);
        }

        I2C_StructInit (&i2cInitStruct);
        i2cInitStruct.I2C_OperatingMode = I2C_OperatingMode_Master;
        i2cInitStruct.I2C_ClockSpeed = clockSpeed;
        I2C_Init (instance, &i2cInitStruct);

        /* Clear all I2C pending interrupts */
        I2C_ClearITPendingBit (instance, I2C_IT_MSK);
}

/*****************************************************************************/

void I2c::read (uint8_t devAddr, uint8_t *data, size_t length, uint16_t timeout)
{
        I2C_TransactionType t;

        /* Prepare to read data */
        t.Operation = I2C_Operation_Read;
        t.Address = devAddr;
        t.StartByte = I2C_StartByte_Disable;
        t.AddressType = I2C_AddressType_7Bit;
        t.StopCondition = I2C_StopCondition_Enable;
        t.Length = 1;

        I2C_BeginTransaction (i2c, &t);

        /* Check read */
        do {
                if (I2C_OP_ABORTED == I2C_GetStatus (i2c)) {
                        Error_Handler ();
                }

        } while (RESET == I2C_GetITStatus (i2c, I2C_IT_MTD));

        I2C_ClearITPendingBit (i2c, I2C_IT_MTD | I2C_IT_MTDWS);

        /* Get data */
        *data = I2C_ReceiveData (i2c);
}

/*****************************************************************************/

void I2c::write (uint8_t devAddr, uint8_t *data, size_t length, uint16_t timeout)
{
        I2C_TransactionType t;

        /* Write I2C device address address */
        t.Operation = I2C_Operation_Write;
        t.Address = devAddr;
        t.StartByte = I2C_StartByte_Disable;
        t.AddressType = I2C_AddressType_7Bit;
        t.StopCondition = I2C_StopCondition_Disable;
        t.Length = 1;

        I2C_FlushTx (i2c);

        /* wait until Flush finished */
        while (I2C_WaitFlushTx (i2c) == I2C_OP_ONGOING)
                ;

        /* Write I2C device address address and put the send_val in TX FIFO */
        I2C_BeginTransaction (i2c, &t);
        I2C_FillTxFIFO (i2c, data[0]);

        /* Check write */
        do {
                if (I2C_OP_ABORTED == I2C_GetStatus (i2c)) {
                        volatile uint32_t cause = i2c->SR_b.CAUSE;
                        Error_Handler ();
                }

        } while (I2C_GetITStatus (i2c, I2C_IT_MTDWS) == RESET);

        I2C_ClearITPendingBit (i2c, I2C_IT_MTDWS);
}

void I2c::read (uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t length, uint16_t timeout)
{
        write (devAddr, &regAddr, 1);
        read (devAddr, data, length);
}

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

        write (devAddr, tmp, length + 1);
}
