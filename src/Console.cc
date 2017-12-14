/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Console.h"
#include "ErrorHandler.h"

/*****************************************************************************/

Console::Console (Usart *u) : usart (u), rxBufferPos (0)
{
        rxBuffer = new char[MAX_RX_BUFFER];
        usart->transmit ((uint8_t *)">", 1);
}

/*****************************************************************************/

void Console::onData (uint8_t c)
{
        if (c != '\n') {
                rxBuffer[rxBufferPos] = c;

                if (++rxBufferPos >= MAX_RX_BUFFER) {
                        Error_Handler ();
                }
        }
        else {
                rxBuffer[rxBufferPos] = '\0';

                if (++rxBufferPos >= MAX_RX_BUFFER) {
                        Error_Handler ();
                }

                // Double buffer. Circullar buffer or sth. Circullar char * like in USB examples.
                onNewLine (rxBuffer, rxBufferPos);
                rxBufferPos = 0;
        }
}

/*****************************************************************************/

void Console::run ()
{

}

/*****************************************************************************/

void Console::onNewLine (const char *str, size_t len)
{
        usart->transmit ((uint8_t *)str, len);
        usart->transmit ((uint8_t *)"\n", 1);
        usart->transmit ((uint8_t *)">", 1);
}
