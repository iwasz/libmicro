/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef SERIALFLASH_H
#define SERIALFLASH_H

#include "Spi.h"

/**
 * Serial flash class developed and tested on LE25U20AMB.
 */
class SerialFlash {
public:
        enum Command {
                READ = 0x03,
                SMALL_SECTOR_ERASE = 0xd7,
                SECTOR_ERASE = 0xd8,
                CHIP_ERASE = 0xc7,
                PAGE_PROGRAM = 0x02,
                WRITE_ENABLE = 0x06,
                WRITE_DISABLE = 0x04,
                POWER_DOWN = 0x0b9,
                STATUS_REGISTER_READ = 0x05,
                STATUS_REGISTER_WRITE = 0x01,
                READ_SILICON_ID1 = 0x9f,
                READ_SILICON_ID2 = 0xab,
                EXIT_POWER_DOWN_MODE = 0xab
        };

        SerialFlash (Spi *s) : spi (s) {}
        void read (uint32_t address);
        uint8_t statusRegisterRead () const;
        void statusRegisterWrite (uint8_t value);

        uint32_t readSiliconId1 () const;

        void writeEnable ();
        void writeDisable ();

private:
        Spi *spi;
};

#endif // SERIALFLASH_H
