/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "SerialFlash.h"
#include "Debug.h"
#include "Timer.h"

/*****************************************************************************/

void SerialFlash::read (uint32_t address, uint8_t *buf, size_t len)
{
        uint16_t b = 0, c = 0;
        while ((b = statusRegisterRead ()) & RDY) {
                Debug::singleton ()->print (b);
                Debug::singleton ()->print ("\n");
                c += b;
                Timer::delay (1);
        }

        Debug::singleton ()->print ("sum : ");
        Debug::singleton ()->print (c);
        Debug::singleton ()->print ("\n");

        spi->setNss (false);
        address <<= 8;
        address |= READ;
        spi->transmit8 (reinterpret_cast<uint8_t *> (&address), 4, nullptr, 50);
        spi->receive8 (buf, len, 50);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::write (uint32_t address, uint8_t const *buf, size_t len)
{
        while (statusRegisterRead () & RDY) {
                Timer::delay (1);
        }

        spi->setNss (false);
        spi->transmit8 (WRITE_ENABLE);
        address <<= 8;
        address |= PAGE_PROGRAM;
        spi->transmit8 (reinterpret_cast<uint8_t *> (&address), 4, nullptr, 50);
        spi->transmit8 (buf, len, nullptr, 50);
        Timer::delay (2);
        spi->setNss (true);
}

/*****************************************************************************/

uint8_t SerialFlash::statusRegisterRead () const
{
        spi->setNss (false);
        spi->transmit8 (STATUS_REGISTER_READ);
        uint8_t ret = spi->transmit8 (0xff);
        spi->setNss (true);
        return ret;
}

/*****************************************************************************/

void SerialFlash::statusRegisterWrite (uint8_t value)
{
        spi->setNss (false);
        spi->transmit8 (STATUS_REGISTER_WRITE);
        spi->transmit8 (value);
        spi->setNss (true);
}

/*****************************************************************************/

uint32_t SerialFlash::readSiliconId1 () const
{
        spi->setNss (false);
        spi->transmit8 (READ_SILICON_ID1);
        uint32_t ret = spi->transmit8 (0xff);
        ret |= static_cast<uint32_t> (spi->transmit8 (0xff)) << 8;
        ret |= static_cast<uint32_t> (spi->transmit8 (0xff)) << 16;
        ret |= static_cast<uint32_t> (spi->transmit8 (0xff)) << 24;
        spi->setNss (true);
        return ret;
}

/*****************************************************************************/

void SerialFlash::writeEnable ()
{
        spi->setNss (false);
        spi->transmit8 (WRITE_ENABLE);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::writeDisable ()
{
        spi->setNss (false);
        spi->transmit8 (WRITE_DISABLE);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::globalBlockProtectionUnlock ()
{
        spi->setNss (false);
        spi->transmit8 (GLOBAL_BLOCK_PROTECTION_UNLOCK);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::chipErase ()
{
        spi->setNss (false);
        spi->transmit8 (WRITE_ENABLE);
        spi->transmit8 (CHIP_ERASE);
        spi->setNss (true);
}
