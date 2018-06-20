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
        while (statusRegisterRead () & RDY)
                ;

        spi->setNss (false);
        address <<= 8;
        address |= READ;
        spi->transmit8 (reinterpret_cast<uint8_t *> (&address), 4, nullptr, 10);
        spi->receive8 (buf, len, 10);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::write (uint32_t address, uint8_t const *buf, size_t len)
{
        while (statusRegisterRead () & RDY)
                ;

        spi->setNss (false);
        address <<= 8;
        address |= PAGE_PROGRAM;
        spi->transmit8 (reinterpret_cast<uint8_t *> (&address), 4, nullptr, 10);
        spi->transmit8 (buf, len, nullptr, 10);
        spi->setNss (true);

        while (statusRegisterRead () & RDY)
                ;
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

/*****************************************************************************/

uint8_t SerialFlash::configRegisterRead () const
{
        spi->setNss (false);
        spi->transmit8 (CONFIGURATION_REGISTER_READ);
        uint8_t ret = spi->transmit8 (0xff);
        spi->setNss (true);
        return ret;
}

/*****************************************************************************/

void SerialFlash::blockProtectionRegisterRead (BlockProtectionRegister &reg) const
{
        spi->setNss (false);
        spi->transmit8 (BLOCK_PROTECTION_REGISTER_READ);
        spi->receive8 (reg, 10, 50);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::sectorErase (uint32_t address, uint8_t eraseInstruction)
{
        while (statusRegisterRead () & RDY)
                ;

        spi->setNss (false);
        address <<= 8;
        address |= eraseInstruction;
        spi->transmit8 (reinterpret_cast<uint8_t *> (&address), 4, nullptr, 10);
        spi->setNss (true);

        while (statusRegisterRead () & RDY)
                ;
}
