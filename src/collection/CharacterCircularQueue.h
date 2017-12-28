/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef CHARACTERCIRCULARQUEUE_H
#define CHARACTERCIRCULARQUEUE_H

#include <ErrorHandler.h>
#include <cstdint>
#include <cstring>
#include <utility>

/**
 * Fancy byte-bassed circular queue which allows zero-copy reads.
 *
 * Usage example:
 *
 * CharacterCircularQueue<128> buf;
 *   buf.pushBack ("quick"));
 *   buf.pushBack ("brown"));
 *   buf.pushBack ("fox"));
 *
 *   REQUIRE (buf.front ().first == std::string ("quick"));
 *
 *   buf.popFront ();
 *   REQUIRE (buf.front ().first == std::string ("brown"));
 *
 *   buf.popFront ();
 *   REQUIRE (buf.front ().first == std::string ("fox"));
 *
 * Description:
 *
 *
 * Limitations:
 * - Elements (i.e. strings you push onto the stack) can be only EOB-1 long,
 *   which currently equals 254 bytes (with '\0'). Of course if MAX_SIZE must be
 *   taken into account.
 * - Number of elements (strings) you can push is limited to 255.
 *
 * Invariants:
 * - input always points to the empty space in buffer, where new data goes.
 * - output always points to the oldest chunk of data (data to be read first).
 *
 * Disclaimer:
 * This collection is not intended to implement STL like behabiour.  It has very
 * limited API, and was created for specified purpose. Thus method names were
 * written in camel case to clearly show this.
 */
template <size_t MAX_SIZE> class CharacterCircularQueue {
public:
        /// End Of Buffer
        enum { EOB = 0xff };

        CharacterCircularQueue () : input (buffer), output (buffer), elementsNo (0) {}

        /**
         * Pushes an element onto the queue. If no space is available, it returns false.
         */
        bool pushBack (char const *s);

        /**
         * Returns first (the oldest) element of the queue and its length. Warning : if collection
         * is empty, this method returns garbage. It is up to the user to check beforehand.
         * Warning 2 : second member of returned pair contains the length of returned string
         * element WITHOUT '\0'.
         */
        std::pair<char const *, uint8_t> front () const;

        /**
         * Removes the oldest element if any. If no elements are available for removing,
         * returns false.
         */
        bool popFront ();

        /**
         * Returns number of elements pushed back. Warning : it does not tell you the
         * number of bytes used to store your elemenets. Only the elements number.
         */
        size_t getSize () const { return elementsNo; }

        /// Returns if the collection is empty.
        bool isEmpty () const { return getSize () == 0; }

private:
        char buffer[MAX_SIZE];
        char *input;
        char *output;
        uint8_t elementsNo;
};

/*****************************************************************************/

template <size_t MAX_SIZE> bool CharacterCircularQueue<MAX_SIZE>::pushBack (char const *s)
{
        size_t len = strlen (s) + 1;

        if (len > EOB - 1) {
                Error_Handler ();
        }

        char *dest;
        char *dataEnd = (output <= input) ? (buffer + MAX_SIZE) : (output);

        // Special case where every byte of 'buffer' is used
        if (input == output && elementsNo) {
                dataEnd = output;
        }

        // Is there free space at the end?
        if (input + len + 1 < dataEnd) {
                dest = input;
                input += len + 1;
        }
        else if (input + len + 1 == dataEnd) {
                dest = input;
                input = buffer;
        }
        // Is here free space on the beginnig?
        else if (buffer + len + 1 < output) {
                *input = EOB;
                dest = buffer;
                input = buffer + len + 1;
        }
        else {
                return false;
        }

        // Another special case, when collection is empty, but output points to garbage
        if (!elementsNo) {
                output = dest;
        }

        *dest = len;
        memcpy (dest + 1, s, len);
        ++elementsNo;
        return true;
}

/*****************************************************************************/

template <size_t MAX_SIZE> std::pair<char const *, uint8_t> CharacterCircularQueue<MAX_SIZE>::front () const
{
        return std::make_pair<char const *, uint8_t> (output + 1, *output - 1);
}

/*****************************************************************************/

template <size_t MAX_SIZE> bool CharacterCircularQueue<MAX_SIZE>::popFront ()
{
        if (!elementsNo) {
                return false;
        }

        --elementsNo;
        size_t len = *output;
        output += len + 1;

        if (output >= buffer + MAX_SIZE || *output == char(EOB)) {
                output = buffer;
        }

        return true;
}

#endif // CHARACTERCIRCULARQUEUE_H
