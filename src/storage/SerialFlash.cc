/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "SerialFlash.h"

void SerialFlash::read (uint32_t address)
{
        spi->setNss (false);
        spi->transmit8 (READ);
        //        spi->transmit8 (value);
        spi->setNss (true);
}

uint8_t SerialFlash::statusRegisterRead () const
{
        spi->setNss (false);
        spi->transmit8 (STATUS_REGISTER_READ);
        uint8_t ret = spi->transmit8 (0xff);
        spi->setNss (true);
        return ret;
}

void SerialFlash::statusRegisterWrite (uint8_t value)
{
        spi->setNss (false);
        spi->transmit8 (STATUS_REGISTER_WRITE);
        spi->transmit8 (value);
        spi->setNss (true);
}

uint32_t SerialFlash::readSiliconId1 () const
{
        spi->setNss (false);
        spi->transmit8 (STATUS_REGISTER_WRITE);
        uint32_t ret = spi->transmit8 (0xff);
        spi->setNss (true);
        return ret;
}

void SerialFlash::writeEnable ()
{
        spi->setNss (false);
        spi->transmit8 (WRITE_ENABLE);
        spi->setNss (true);
}

void SerialFlash::writeDisable ()
{
        spi->setNss (false);
        spi->transmit8 (WRITE_DISABLE);
        spi->setNss (true);
}
