/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ACCELERO_LSM6ds3_H
#define ACCELERO_LSM6ds3_H

#include "IAccelerometer.h"
#include "IGyroscope.h"
#include "ILsm6ds3Bsp.h"
#include <cstdint>

/**
 * Lsm6ds3 API the way I like it.
 */
class Lsm6ds3 : public IAccelerometer, public IGyroscope {
public:
        Lsm6ds3 (ILsm6ds3Bsp *b) : bsp (b) {}
        virtual ~Lsm6ds3 () {}

        /*---------------------------------------------------------------------------*/

        AData getAData () const;
        AData getRawAData () const;
        GData getGData () const;
        GData getRawGData () const;

        /*****************************************************************************/
        uint8_t readRegister (uint8_t r) { return bsp->readRegister (r); }
        /*---------------------------------------------------------------------------*/

        /// Should return 0x69
        uint8_t getWhoAmI () const { return bsp->readRegister (LSM6DS3_ACC_GYRO_WHO_AM_I_REG); }

        /*---------------------------------------------------------------------------*/

        /// Block Data Update
        enum Bdu { CONTINUOS = 0x00, BLOCK_UPDATE = 0x40, BDU_MASK = 0x40 };

        Bdu getBdu () const { return static_cast<Bdu> (bsp->readRegister (LSM6DS3_ACC_GYRO_CTRL3_C) & BDU_MASK); }
        void setBdu (Bdu b) { writeRegister (LSM6DS3_ACC_GYRO_CTRL3_C, BDU_MASK, b); }

        /*---------------------------------------------------------------------------*/

        enum AccelFullScale { FS_2G = 0x00, FS_16G = 0x04, FS_4G = 0x08, FS_8G = 0x0C, ACCEL_FS_MASK = 0x0c };

        AccelFullScale getAccelFullScale () const
        {
                return static_cast<AccelFullScale> (bsp->readRegister (LSM6DS3_ACC_GYRO_CTRL1_XL) & ACCEL_FS_MASK);
        }
        void setAccelFullScale (AccelFullScale a) { writeRegister (LSM6DS3_ACC_GYRO_CTRL1_XL, ACCEL_FS_MASK, a); }

        /*---------------------------------------------------------------------------*/

        enum AccelOdr {
                ACCEL_ODR_POWER_DOWN = 0x00,
                ACCEL_ODR_13Hz = 0x10,
                ACCEL_ODR_26Hz = 0x20,
                ACCEL_ODR_52Hz = 0x30,
                ACCEL_ODR_104Hz = 0x40,
                ACCEL_ODR_208Hz = 0x50,
                ACCEL_ODR_416Hz = 0x60,
                ACCEL_ODR_833Hz = 0x70,
                ACCEL_ODR_1660Hz = 0x80,
                ACCEL_ODR_3330Hz = 0x90,
                ACCEL_ODR_6660Hz = 0xA0,
                ACCEL_ODR_MASK = 0xF0
        };

        AccelOdr getAccelOdr () const { return static_cast<AccelOdr> (bsp->readRegister (LSM6DS3_ACC_GYRO_CTRL1_XL) & ACCEL_ODR_MASK); }
        void setAccelOdr (AccelOdr b) { writeRegister (LSM6DS3_ACC_GYRO_CTRL1_XL, ACCEL_ODR_MASK, b); }

        /*---------------------------------------------------------------------------*/

        enum AntiAliasingBandwidth { BW_400Hz = 0x00, BW_200Hz = 0x01, BW_100Hz = 0x02, BW_50Hz = 0x03, BW_MASK = 0x03 };
        AntiAliasingBandwidth getAntiAliasingBandwidth () const
        {
                return static_cast<AntiAliasingBandwidth> (bsp->readRegister (LSM6DS3_ACC_GYRO_CTRL1_XL) & BW_MASK);
        }

        void setAntiAliasingBandwidth (AntiAliasingBandwidth b) { writeRegister (LSM6DS3_ACC_GYRO_CTRL1_XL, BW_MASK, b); }

        /*****************************************************************************/

        enum GyroFullScale { FS_245dps = 0x00, FS_500dps = 0x04, FS_1000dps = 0x08, FS_2000dps = 0x0C, GYRO_FS_MASK = 0x0c };

        GyroFullScale getGyroFullScale () const
        {
                return static_cast<GyroFullScale> (bsp->readRegister (LSM6DS3_ACC_GYRO_CTRL2_G) & GYRO_FS_MASK);
        }
        void setGyroFullScale (GyroFullScale a) { writeRegister (LSM6DS3_ACC_GYRO_CTRL2_G, GYRO_FS_MASK, a); }

        /*---------------------------------------------------------------------------*/

        enum GyroOdr {
                GYRO_ODR_POWER_DOWN = 0x00,
                GYRO_ODR_13Hz = 0x10,
                GYRO_ODR_26Hz = 0x20,
                GYRO_ODR_52Hz = 0x30,
                GYRO_ODR_104Hz = 0x40,
                GYRO_ODR_208Hz = 0x50,
                GYRO_ODR_416Hz = 0x60,
                GYRO_ODR_833Hz = 0x70,
                GYRO_ODR_1660Hz = 0x80,
                GYRO_ODR_MASK = 0xF0
        };

        GyroOdr getGyroOdr () const { return static_cast<GyroOdr> (bsp->readRegister (LSM6DS3_ACC_GYRO_CTRL2_G) & GYRO_ODR_MASK); }
        void setGyroOdr (GyroOdr b) { writeRegister (LSM6DS3_ACC_GYRO_CTRL2_G, GYRO_ODR_MASK, b); }

        /*---------------------------------------------------------------------------*/

        /// Gyroscope full-scale at 125 dps. Default value: 0
        enum FS_125_DPS { FS_125_DISABLED = 0x00, FS_125_ENABLED = 0x02, FS_125_MASK = 0x02 };
        bool getFs125Dps () const { return static_cast<GyroOdr> (bsp->readRegister (LSM6DS3_ACC_GYRO_CTRL2_G) & FS_125_MASK); }
        void setFs125Dps (bool b) { writeRegister (LSM6DS3_ACC_GYRO_CTRL2_G, FS_125_MASK, (b) ? (FS_125_ENABLED) : (FS_125_DISABLED)); }

        /*****************************************************************************/

        enum I2cOperation { I2C_AND_SPI = 0x00, SPI_ONLY = 0x04, I2C_DISABLE_MASK = 0x04 };
        bool getI2cEnable () const { return bsp->readRegister (LSM6DS3_ACC_GYRO_CTRL4_C) & I2C_DISABLE_MASK; }
        void setI2cEnable (bool b) { writeRegister (LSM6DS3_ACC_GYRO_CTRL4_C, I2C_DISABLE_MASK, (b) ? (I2C_AND_SPI) : (SPI_ONLY)); };

        /*****************************************************************************/

        /// Checks for interrupt status DRDY
        //        bool isDataReady () const;

        // Config
        //        void interruptConfig (Lsm6ds3InterruptConfig const &LIS3DSH_IntConfigStruct);
        //        void clickITConfig ();

private:
        enum Register {
                LSM6DS3_ACC_GYRO_FUNC_CFG_ACCESS = 0x01,
                LSM6DS3_ACC_GYRO_SENSOR_SYNC_TIME = 0x04,
                LSM6DS3_ACC_GYRO_FIFO_CTRL1 = 0x06,
                LSM6DS3_ACC_GYRO_FIFO_CTRL2 = 0x07,
                LSM6DS3_ACC_GYRO_FIFO_CTRL3 = 0x08,
                LSM6DS3_ACC_GYRO_FIFO_CTRL4 = 0x09,
                LSM6DS3_ACC_GYRO_FIFO_CTRL5 = 0x0A,
                LSM6DS3_ACC_GYRO_ORIENT_CFG_G = 0x0B,
                LSM6DS3_ACC_GYRO_INT1_CTRL = 0x0D,
                LSM6DS3_ACC_GYRO_INT2_CTRL = 0x0E,
                LSM6DS3_ACC_GYRO_WHO_AM_I_REG = 0x0F,
                LSM6DS3_ACC_GYRO_CTRL1_XL = 0x10,
                LSM6DS3_ACC_GYRO_CTRL2_G = 0x11,
                LSM6DS3_ACC_GYRO_CTRL3_C = 0x12,
                LSM6DS3_ACC_GYRO_CTRL4_C = 0x13,
                LSM6DS3_ACC_GYRO_CTRL5_C = 0x14,
                LSM6DS3_ACC_GYRO_CTRL6_G = 0x15,
                LSM6DS3_ACC_GYRO_CTRL7_G = 0x16,
                LSM6DS3_ACC_GYRO_CTRL8_XL = 0x17,
                LSM6DS3_ACC_GYRO_CTRL9_XL = 0x18,
                LSM6DS3_ACC_GYRO_CTRL10_C = 0x19,
                LSM6DS3_ACC_GYRO_MASTER_CONFIG = 0x1A,
                LSM6DS3_ACC_GYRO_WAKE_UP_SRC = 0x1B,
                LSM6DS3_ACC_GYRO_TAP_SRC = 0x1C,
                LSM6DS3_ACC_GYRO_D6D_SRC = 0x1D,
                LSM6DS3_ACC_GYRO_STATUS_REG = 0x1E,
                LSM6DS3_ACC_GYRO_OUT_TEMP_L = 0x20,
                LSM6DS3_ACC_GYRO_OUT_TEMP_H = 0x21,
                LSM6DS3_ACC_GYRO_OUTX_L_G = 0x22,
                LSM6DS3_ACC_GYRO_OUTX_H_G = 0x23,
                LSM6DS3_ACC_GYRO_OUTY_L_G = 0x24,
                LSM6DS3_ACC_GYRO_OUTY_H_G = 0x25,
                LSM6DS3_ACC_GYRO_OUTZ_L_G = 0x26,
                LSM6DS3_ACC_GYRO_OUTZ_H_G = 0x27,
                LSM6DS3_ACC_GYRO_OUTX_L_XL = 0x28,
                LSM6DS3_ACC_GYRO_OUTX_H_XL = 0x29,
                LSM6DS3_ACC_GYRO_OUTY_L_XL = 0x2A,
                LSM6DS3_ACC_GYRO_OUTY_H_XL = 0x2B,
                LSM6DS3_ACC_GYRO_OUTZ_L_XL = 0x2C,
                LSM6DS3_ACC_GYRO_OUTZ_H_XL = 0x2D,
                LSM6DS3_ACC_GYRO_SENSORHUB1_REG = 0x2E,
                LSM6DS3_ACC_GYRO_SENSORHUB2_REG = 0x2F,
                LSM6DS3_ACC_GYRO_SENSORHUB3_REG = 0x30,
                LSM6DS3_ACC_GYRO_SENSORHUB4_REG = 0x31,
                LSM6DS3_ACC_GYRO_SENSORHUB5_REG = 0x32,
                LSM6DS3_ACC_GYRO_SENSORHUB6_REG = 0x33,
                LSM6DS3_ACC_GYRO_SENSORHUB7_REG = 0x34,
                LSM6DS3_ACC_GYRO_SENSORHUB8_REG = 0x35,
                LSM6DS3_ACC_GYRO_SENSORHUB9_REG = 0x36,
                LSM6DS3_ACC_GYRO_SENSORHUB10_REG = 0x37,
                LSM6DS3_ACC_GYRO_SENSORHUB11_REG = 0x38,
                LSM6DS3_ACC_GYRO_SENSORHUB12_REG = 0x39,
                LSM6DS3_ACC_GYRO_FIFO_STATUS1 = 0x3A,
                LSM6DS3_ACC_GYRO_FIFO_STATUS2 = 0x3B,
                LSM6DS3_ACC_GYRO_FIFO_STATUS3 = 0x3C,
                LSM6DS3_ACC_GYRO_FIFO_STATUS4 = 0x3D,
                LSM6DS3_ACC_GYRO_FIFO_DATA_OUT_L = 0x3E,
                LSM6DS3_ACC_GYRO_FIFO_DATA_OUT_H = 0x3F,
                LSM6DS3_ACC_GYRO_TIMESTAMP0_REG = 0x40,
                LSM6DS3_ACC_GYRO_TIMESTAMP1_REG = 0x41,
                LSM6DS3_ACC_GYRO_TIMESTAMP2_REG = 0x42,
                LSM6DS3_ACC_GYRO_TIMESTAMP_L = 0x49,
                LSM6DS3_ACC_GYRO_TIMESTAMP_H = 0x4A,
                LSM6DS3_ACC_GYRO_STEP_COUNTER_L = 0x4B,
                LSM6DS3_ACC_GYRO_STEP_COUNTER_H = 0x4C,
                LSM6DS3_ACC_GYRO_SENSORHUB13_REG = 0x4D,
                LSM6DS3_ACC_GYRO_SENSORHUB14_REG = 0x4E,
                LSM6DS3_ACC_GYRO_SENSORHUB15_REG = 0x4F,
                LSM6DS3_ACC_GYRO_SENSORHUB16_REG = 0x50,
                LSM6DS3_ACC_GYRO_SENSORHUB17_REG = 0x51,
                LSM6DS3_ACC_GYRO_SENSORHUB18_REG = 0x52,
                LSM6DS3_ACC_GYRO_FUNC_SRC = 0x53,
                LSM6DS3_ACC_GYRO_TAP_CFG1 = 0x58,
                LSM6DS3_ACC_GYRO_TAP_THS_6D = 0x59,
                LSM6DS3_ACC_GYRO_INT_DUR2 = 0x5A,
                LSM6DS3_ACC_GYRO_WAKE_UP_THS = 0x5B,
                LSM6DS3_ACC_GYRO_WAKE_UP_DUR = 0x5C,
                LSM6DS3_ACC_GYRO_FREE_FALL = 0x5D,
                LSM6DS3_ACC_GYRO_MD1_CFG = 0x5E,
                LSM6DS3_ACC_GYRO_MD2_CFG = 0x5F
        };

        /**
         * Clears the mask, writes value.
         */
        void writeRegister (Register reg, uint8_t mask, uint8_t value);

        enum FunctionRegister {
                LSM6DS3_ACC_GYRO_SLV0_ADD = 0x02,
                LSM6DS3_ACC_GYRO_SLV0_SUBADD = 0x03,
                LSM6DS3_ACC_GYRO_SLAVE0_CONFIG = 0x04,
                LSM6DS3_ACC_GYRO_SLV1_ADD = 0x05,
                LSM6DS3_ACC_GYRO_SLV1_SUBADD = 0x06,
                LSM6DS3_ACC_GYRO_SLAVE1_CONFIG = 0x07,
                LSM6DS3_ACC_GYRO_SLV2_ADD = 0x08,
                LSM6DS3_ACC_GYRO_SLV2_SUBADD = 0x09,
                LSM6DS3_ACC_GYRO_SLAVE2_CONFIG = 0x0A,
                LSM6DS3_ACC_GYRO_SLV3_ADD = 0x0B,
                LSM6DS3_ACC_GYRO_SLV3_SUBADD = 0x0C,
                LSM6DS3_ACC_GYRO_SLAVE3_CONFIG = 0x0D,
                LSM6DS3_ACC_GYRO_DATAWRITE_SRC_MODE_SUB_SLV0 = 0x0E,
                LSM6DS3_ACC_GYRO_CONFIG_PEDO_THS_MIN = 0x0F,
                LSM6DS3_ACC_GYRO_SM_STEP_THS = 0x13,
                LSM6DS3_ACC_GYRO_PEDO_DEB_REG = 0x14,
                LSM6DS3_ACC_GYRO_STEP_COUNT_DELTA = 0x15,
                LSM6DS3_ACC_GYRO_MAG_SI_XX = 0x24,
                LSM6DS3_ACC_GYRO_MAG_SI_XY = 0x25,
                LSM6DS3_ACC_GYRO_MAG_SI_XZ = 0x26,
                LSM6DS3_ACC_GYRO_MAG_SI_YX = 0x27,
                LSM6DS3_ACC_GYRO_MAG_SI_YY = 0x28,
                LSM6DS3_ACC_GYRO_MAG_SI_YZ = 0x29,
                LSM6DS3_ACC_GYRO_MAG_SI_ZX = 0x2A,
                LSM6DS3_ACC_GYRO_MAG_SI_ZY = 0x2B,
                LSM6DS3_ACC_GYRO_MAG_SI_ZZ = 0x2C,
                LSM6DS3_ACC_GYRO_MAG_OFFX_L = 0x2D,
                LSM6DS3_ACC_GYRO_MAG_OFFX_H = 0x2E,
                LSM6DS3_ACC_GYRO_MAG_OFFY_L = 0x2F,
                LSM6DS3_ACC_GYRO_MAG_OFFY_H = 0x30,
                LSM6DS3_ACC_GYRO_MAG_OFFZ_L = 0x31,
                LSM6DS3_ACC_GYRO_MAG_OFFZ_H = 0x32
        };

private:
        ILsm6ds3Bsp *bsp;
};

#endif // LIS3DSH_H
