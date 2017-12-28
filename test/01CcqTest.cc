/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include "collection/CharacterCircularQueue.h"
#include <string>

/*****************************************************************************/

/**
 * @brief
 */
TEST_CASE ("push", "[ccq2]")
{
        CharacterCircularQueue<16> buf;
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
        CharacterCircularQueue<16> buf;

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
        REQUIRE (buf.front ().second == 3);
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
        CharacterCircularQueue<9> buf;

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
        CharacterCircularQueue<9> buf;

        // Push so much, that all the space in buffer is used.
        REQUIRE (buf.pushBack ("1234567"));
        REQUIRE (buf.popFront ());
        REQUIRE (!buf.pushBack ("12345678"));
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push and remove at once", "[ccq2]")
{
        CharacterCircularQueue<9> buf;

        /* io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("12"));

        /*  o       i
         * +-+-+-+-+-+-+-+-+-+
         * |3|1|2|0| | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("12"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        /*         io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("34"));

        /*          o       i
         * +-+-+-+-+-+-+-+-+-+
         * | | | | |2|3|4|0| |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("34"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        /*                 io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("45"));
        REQUIRE (buf.front ().first == std::string ("45"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("67"));
        REQUIRE (buf.front ().first == std::string ("67"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());
}

TEST_CASE ("push and remove instantly", "[ccq2]")
{
        CharacterCircularQueue<256> buf;

        // Push so much, that all the space in buffer is used.
        REQUIRE (buf.pushBack ("111111111111111111111111111111111111111111111111111111111111111111111111"));
        REQUIRE (buf.front ().first == std::string ("111111111111111111111111111111111111111111111111111111111111111111111111"));
        REQUIRE (buf.front ().second == 72);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("22222222222222222222222222222222222222222222222222222222222222222222222222"));
        REQUIRE (buf.front ().first == std::string ("22222222222222222222222222222222222222222222222222222222222222222222222222"));
        REQUIRE (buf.front ().second == 74);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("3333333333333333333333333333333333333333333333333333333333333333333333333333333"));
        REQUIRE (buf.front ().first == std::string ("3333333333333333333333333333333333333333333333333333333333333333333333333333333"));
        REQUIRE (buf.front ().second == 79);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("44444444444444444444444444444444444444444444444444444444444444444444444444444444"));
        REQUIRE (buf.front ().first == std::string ("44444444444444444444444444444444444444444444444444444444444444444444444444444444"));
        REQUIRE (buf.front ().second == 80);
        REQUIRE (buf.popFront ());
}
