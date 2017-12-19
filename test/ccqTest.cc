/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/
#if 0
#include "catch.hpp"
#include <ErrorHandler.h>
#include <cstring>
#include <utility>

/**
 * Like std::deque <std::vector <T>> but circular.
 */
template <typename T, size_t MAX_STORAGE_SIZE, size_t MAX_SIZE> class ContinuousCircularQueue {
public:
        ContinuousCircularQueue () : end (nullptr), frontp (0), backp (MAX_SIZE - 1), sizep (0) {}

        bool pushBack (T const *s, size_t len);
        std::pair<T const *, size_t> popFront ();
        size_t getSize () const { return sizep; }
        bool isEmpty () const { return getSize () == 0; }
        void clear ();

private:
        T storage[MAX_STORAGE_SIZE];
        T *end;

        struct Slot {
                T *start;
                size_t len;
        };

        Slot queue[MAX_SIZE];
        size_t frontp;
        size_t backp;
        size_t sizep;
};

/*****************************************************************************/

template <typename T, size_t MAX_STORAGE_SIZE, size_t MAX_SIZE> bool ContinuousCircularQueue<T, MAX_STORAGE_SIZE, MAX_SIZE>::pushBack (T const *s, size_t len)
{
        if (sizep >= MAX_SIZE) {
                return false;
        }

        T *p;

//        if (sizep) {
//                p = queue[backp].start + queue[backp].len;
//        }
//        else {
//                p =
//        }

//        memcpy(queue _ x, s, sizeof (T)*len);

                backp
                = (backp + 1) % MAX_SIZE;
        //        queue[backp] = Element ();
        // No tu kopiowanie
        ++sizep;
        return true;
}

/*****************************************************************************/

template <typename T, size_t MAX_STORAGE_SIZE, size_t MAX_SIZE> std::pair<T const *, size_t> ContinuousCircularQueue<T, MAX_STORAGE_SIZE, MAX_SIZE>::popFront ()
{
        if (!getSize ()) {
                return false;
        }

        Slot s = &queue[frontp % MAX_SIZE];

        frontp = (frontp + 1) % MAX_SIZE;
        sizep -= 1;

        return std::pair<T const *, size_t> (s.start, s.len);
}

/*****************************************************************************/

template <typename T, size_t MAX_STORAGE_SIZE, size_t MAX_SIZE> void ContinuousCircularQueue<T, MAX_STORAGE_SIZE, MAX_SIZE>::clear ()
{
        frontp = 0;
        backp = MAX_SIZE - 1;
        sizep = 0;
}

/**
 * @brief
 */
TEST_CASE ("Empty", "[ccq]")
{
        ContinuousCircularQueue<char, 16, 4> buf;
        REQUIRE (buf.isEmpty ());
}

/**
 * Tests size, but does not take storage into account. Only queue.
 */
TEST_CASE ("Size1", "[ccq]")
{
        ContinuousCircularQueue<char, 16, 4> buf;
        REQUIRE (buf.getSize () == 0);
        REQUIRE (buf.pushBack ("a", 2));
        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.pushBack ("k", 2));
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.pushBack ("1", 2));
        REQUIRE (buf.getSize () == 3);
        REQUIRE (buf.pushBack ("2", 2));
        REQUIRE (buf.getSize () == 4);
        REQUIRE (!buf.pushBack ("2", 2));
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("Size", "[ccq]")
{
        ContinuousCircularQueue<char, 16, 4> buf;
        REQUIRE (buf.getSize () == 0);
        REQUIRE (buf.pushBack ("ala", 4));
        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.pushBack ("kot", 4));
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.pushBack ("12345", 6));
        REQUIRE (buf.getSize () == 3);
        REQUIRE (!buf.pushBack ("12345", 6));
}
#endif
