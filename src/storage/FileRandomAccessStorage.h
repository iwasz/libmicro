/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef FILERANDOMACCESSSTORAGE_H
#define FILERANDOMACCESSSTORAGE_H

#include "FlashEepromStorage.h"
#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * @brief Example implenentation of IStorege interface using files, but emulating
 * flash which emulates eeprom :D Remarks : although this class has EEPROM like interface
 * (it was meant to implement eeprom emulated in flash) it is most efficient (and thus
 * recommended) to cache your record (of size 'capacity') in RAM, and store it as a whole
 * using store (data, "capacity_bytes", 0). Remember, that every time you invoke store (even
 * with length == 1) you actually append whole record (of size capacity + WRITE_SIZE) to
 * flash.
 * @param PAGE_SIZE Size of a flash page in bytes.
 * @param WRITE_SIZE Number of bytes that can be simultaneously written to the flash.
 */
template <size_t PAGE_SIZE, size_t WRITE_SIZE = 2> class FileRandomAccessStorage : public FlashEepromStorage<PAGE_SIZE, WRITE_SIZE> {
public:
        /**
         * @brief Constructor
         * @param capacity Size of a logical record which will be stored. For EEPROM memories it would be
         * the memory size, but since we emulate EEPROM in FLASH, we need to know how much of the usable
         * memory will be implented in much bigger region of FLASH. Then, every time you change at least
         * 1 word (WRITE_SIZE) of that memory, whole new record will be append to the FLASH.
         * @param numOfPages
         * @param address
         */
        FileRandomAccessStorage (size_t capacity, size_t numOfPages, size_t address) : FlashEepromStorage<PAGE_SIZE, WRITE_SIZE> (capacity, numOfPages, address)
        {
        }
        virtual ~FileRandomAccessStorage ();

        virtual void init ();

        //        virtual size_t getCapacity () const { return capacity; }

        //        /**
        //         * Remarks : for flash, the most efficient scenario is when length == capacity. If length <
        //         * capacity, then this implementation would read prevoius record, modify it, and store as a
        //         * whole.
        //         */

        //        /**
        //         * @param offset Where to save the data (in bytes counted from the start of the record). Remember that the record
        //         * has a length of 'capacity' bytes. Do not confuse offset with flash memory address. It is less than that.
        //         */
        //        virtual void store (uint8_t *data, size_t length, size_t offset);
        //        virtual uint8_t const *read (uint8_t *out, size_t length, size_t offset, size_t history);
        //        virtual uint8_t const *read (uint8_t *out, size_t length, size_t offset) { return read (out, length, offset, 0); }
        //        virtual void clear ();

protected:
        /// Clear one page.
        virtual void clearPage (size_t address);
        /// Store 1 word (WRITE_SIZE bytes long).
        virtual void storeWordImpl (uint8_t const *word, size_t address);

private:
        /// Do the book keeping after a single word was stored.
        //        void increaseAndClear ();
        //        void storeImpl (uint8_t *data, size_t length, size_t offset);

        //        /**
        //         * @brief Store one word of size WRITE_SIZE, and increase currentPage and currentOffset accordingly.
        //         * If page boundary is exceeded, new page will be used, and if last page is full, first page will be
        //         * cleared and used and so on.
        //         */
        //        void storeWord (uint8_t const *word);

private:
        int fd;
};

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FileRandomAccessStorage<PAGE_SIZE, WRITE_SIZE>::init ()
{
        fd = open ("file-flash-eeprom.bin", O_RDWR, 0666);

        if (fd < 0) {
                Error_Handler ();
        }

        if (ftruncate (fd, PAGE_SIZE * FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::numOfPages) < 0) {
                Error_Handler ();
        }

        FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::contents
                = (uint8_t *)mmap (NULL, PAGE_SIZE * FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::numOfPages, PROT_READ, MAP_SHARED, fd, 0);

        if (!FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::contents) {
                Error_Handler ();
        }

        // Init END_MARKER
        for (size_t i = 0; i < WRITE_SIZE; ++i) {
                FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::END_MARKER[i] = 0xff;
        }

        /*---------------------------------------------------------------------------*/

        FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::initOffsets ();
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> FileRandomAccessStorage<PAGE_SIZE, WRITE_SIZE>::~FileRandomAccessStorage ()
{
        if (munmap (FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::contents, PAGE_SIZE * FlashEepromStorage<PAGE_SIZE, WRITE_SIZE>::numOfPages) < 0) {
                Error_Handler ();
        }

        errno = 0;
        if (close (fd) < 0) {
                Error_Handler ();
        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FileRandomAccessStorage<PAGE_SIZE, WRITE_SIZE>::storeWordImpl (uint8_t const *word, size_t address)
{
        if (lseek (fd, address, SEEK_SET) < 0) {
                Error_Handler ();
        }

        if (write (fd, word, WRITE_SIZE) < (ssize_t)WRITE_SIZE) {
                Error_Handler ();
        }
}

/*****************************************************************************/

template <size_t PAGE_SIZE, size_t WRITE_SIZE> void FileRandomAccessStorage<PAGE_SIZE, WRITE_SIZE>::clearPage (size_t address)
{
        if (lseek (fd, address, SEEK_SET) < 0) {
                Error_Handler ();
        }

        static uint8_t ZERO[PAGE_SIZE] = {
                0,
        };

        if (write (fd, ZERO, PAGE_SIZE) < (ssize_t)PAGE_SIZE) {
                Error_Handler ();
        }
}

#endif // FILERANDOMACCESSSTORAGE_H
