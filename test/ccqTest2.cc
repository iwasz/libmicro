/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include <ErrorHandler.h>
#include <cstring>
#include <string>
#include <utility>

/**
 * Fancy byte-bassed circular queue which allows zero-copy reads.
 *
 * Usage example:
 *
 * Description:
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
template <size_t MAX_SIZE> class ContinuousCircularQueue {
public:
        /// End Of Buffer
        enum { EOB = 0xff };

        ContinuousCircularQueue () : input (buffer), output (buffer), elementsNo (0) {}

        bool pushBack (char const *s);
        std::pair<char const *, uint8_t> front () const;
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

template <size_t MAX_SIZE> bool ContinuousCircularQueue<MAX_SIZE>::pushBack (char const *s)
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

        *dest = len;
        memcpy (dest + 1, s, len);
        ++elementsNo;
        return true;
}

/*****************************************************************************/

template <size_t MAX_SIZE> std::pair<char const *, uint8_t> ContinuousCircularQueue<MAX_SIZE>::front () const
{
        return std::make_pair<char const *, uint8_t> (output + 1, *output);
}

/*****************************************************************************/

template <size_t MAX_SIZE> bool ContinuousCircularQueue<MAX_SIZE>::popFront ()
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

/*****************************************************************************/

/**
 * @brief
 */
TEST_CASE ("push", "[ccq2]")
{
        ContinuousCircularQueue<16> buf;
        buf.pushBack ("ala");
        REQUIRE (buf.front ().first == std::string ("ala"));

        buf.pushBack ("ma");
        REQUIRE (buf.front ().first == std::string ("ala"));
}

/**
 * @brief
 */
TEST_CASE ("push+pop", "[ccq2]")
{
        ContinuousCircularQueue<16> buf;

        /* io
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("ala"));

        /*  o         i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |4|a|l|a|0| | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ala"));
        REQUIRE (buf.pushBack ("ma"));

        /*  o                 i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |4|a|l|a|0|3|m|a|0| | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ala"));
        REQUIRE (buf.popFront ());

        /*            o       i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | |3|m|a|0| | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ma"));
        REQUIRE (buf.pushBack ("kota"));

        /*            o                   i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | |3|m|a|0|5|k|o|t|a|0| |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ma"));
        REQUIRE (buf.popFront ());

        /*                    o           i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |5|k|o|t|a|0| |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("kota"));
        REQUIRE (buf.pushBack ("oraz"));

        /*              i     o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0| | | |5|k|o|t|a|0|E|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("kota"));
        REQUIRE (buf.popFront ());

        /*  o           i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0| | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (buf.pushBack ("psaA"));

        /*  o                       i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0|5|p|s|a|A|0| | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (!buf.pushBack ("abc"));
        REQUIRE (buf.pushBack ("ab"));

        /* io
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0|5|p|s|a|A|0|3|a|b|0|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (!buf.pushBack ("ab"));
        REQUIRE (buf.popFront ());

        /*  i           o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | |5|p|s|a|A|0|3|a|b|0|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("psaA"));
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push+pop2", "[ccq2]")
{
        ContinuousCircularQueue<9> buf;

        REQUIRE (buf.isEmpty ());
        REQUIRE (buf.pushBack ("a"));
        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.pushBack ("b"));
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.pushBack ("c"));
        REQUIRE (buf.getSize () == 3);
        REQUIRE (!buf.pushBack ("d"));

        REQUIRE (buf.front ().first == std::string ("a"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.front ().first == std::string ("b"));
        REQUIRE (buf.pushBack ("d"));
        REQUIRE (buf.getSize () == 3);

        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.front ().first == std::string ("c"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.front ().first == std::string ("d"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 0);
        REQUIRE (buf.isEmpty ());
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push to much", "[ccq2]")
{
        ContinuousCircularQueue<9> buf;

        // Push so much, that all the space in buffer is used.
        REQUIRE (buf.pushBack ("1234567"));
        REQUIRE (buf.popFront ());
        REQUIRE (!buf.pushBack ("12345678"));
}
