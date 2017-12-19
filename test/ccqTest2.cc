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
 *
 * Invariants:
 * - input always points to the empty space in buffer, where new data goes.
 * - output always points to the oldest chunk of data (data to be read first).
 */
template <size_t MAX_SIZE> class ContinuousCircularQueue {
public:
        // End Of Buffer
        enum { EOB = 0xff };

        ContinuousCircularQueue () : input (buffer), output (buffer) /*, empty (false)*/ {}

        bool pushBack (char const *s);
        std::pair<char const *, uint8_t> front () const;
        void popFront ();
        size_t getSize () const;
        bool isEmpty () const { return getSize () == 0; }

        //        void clear ();

private:
        char buffer[MAX_SIZE];
        char *input;
        char *output;
        //        bool empty;
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
                Error_Handler ();
        }

        *dest = len;
        memcpy (dest + 1, s, len);
        return true;
}

/*****************************************************************************/

template <size_t MAX_SIZE> std::pair<char const *, uint8_t> ContinuousCircularQueue<MAX_SIZE>::front () const
{
        return std::make_pair<char const *, uint8_t> (output + 1, *output);
}

/*****************************************************************************/

template <size_t MAX_SIZE> void ContinuousCircularQueue<MAX_SIZE>::popFront ()
{
        size_t len = *output;
        output += len + 1;

        if (output > buffer + MAX_SIZE || *output == char(EOB)) {
                output = buffer;
        }
}

/*****************************************************************************/

template <size_t MAX_SIZE> size_t ContinuousCircularQueue<MAX_SIZE>::getSize () const
{
        //        if (empty) {
        //                return 0;
        //        }

        //        if (input > output) {
        //                return input - output;
        //        }

        //        return MAX_SIZE - output + input;
}

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
        buf.popFront ();

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
        buf.popFront ();

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
        buf.popFront ();

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

        /*  o                               i?
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0|5|p|s|a|A|0|3|a|b|0|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */
}
