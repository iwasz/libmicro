/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Debug.h"
#include "ErrorHandler.h"
#include "Hal.h"
#include "Usart.h"
#include <cstring>

Debug *Debug::instance;

Debug *&Debug::singleton () { return instance; }

/*****************************************************************************/

void Debug::print (const char *str)
{
        // TODO DMA
        size_t len = strlen (str);
        uart->transmit ((uint8_t *)str, len);
}

/*****************************************************************************/

void Debug::println (const char *str)
{
        print (str);
        print ("\n");
}

/*****************************************************************************/

void Debug::print (uint8_t *data, size_t len)
{
        // TODO DMA
        uart->transmit (data, len);
}

/*****************************************************************************/

void Debug::printArray (uint8_t *data, size_t len)
{
        for (size_t i = 0; i < len; ++i) {
                print (data[i]);

                if (i < len - 1) {
                        print (",");
                }
        }
}

/* reverse:  reverse string s in place */
static void reverse (char s[])
{
        int i, j;
        char c;

        for (i = 0, j = strlen (s) - 1; i < j; i++, j--) {
                c = s[i];
                s[i] = s[j];
                s[j] = c;
        }
}

/* itoa:  convert n to characters in s */
void itoa (int n, char s[], int zeroPad = 0)
{
        int i, sign;

        if ((sign = n) < 0) { /* record sign */
                n = -n;       /* make n positive */
        }

        i = 0;

        do {                           /* generate digits in reverse order */
                s[i++] = n % 10 + '0'; /* get next digit */
        } while ((n /= 10) > 0);       /* delete it */

        for (; i < zeroPad; ++i) {
                s[i] = '0';
        }

        if (sign < 0) {
                s[i++] = '-';
        }

        s[i] = '\0';

        reverse (s);
}

void Debug::print (int i)
{
        char buf[11];
        itoa (i, buf);
        print (buf);
}

/*****************************************************************************/

void Debug::printTime (uint16_t time)
{
        char buf[6];
        uint16_t sec100 = time % 100;

        time /= 100;
        uint16_t sec = time % 60;

        time /= 60;
        uint16_t min = time % 60;

        itoa (min, buf);
        print (buf);
        print (":");

        itoa (sec, buf, 2);
        print (buf);
        print (",");

        itoa (sec100, buf, 2);
        print (buf);
}

/*****************************************************************************/

extern "C" void debugPrint (uint8_t *data, size_t len) { Debug::singleton ()->print (data, len); }
