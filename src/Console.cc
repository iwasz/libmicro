/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Console.h"
#include "ErrorHandler.h"
#include <utility>

/*****************************************************************************/

Console::Console () : input (nullptr), output (nullptr), rxBufferPos (0) { rxBuffer = new char[MAX_RX_BUFFER]; }

/*****************************************************************************/

void Console::setInput (ICharacterInput *i)
{
        input = i;
        input->setSink (this);
}

/*****************************************************************************/

void Console::setOutput (ICharacterOutput *o)
{
        o->transmit ("$");
        output = o;
}

/*****************************************************************************/

void Console::onData (char c)
{
        if (c != '\n' && c != '\r') {
                rxBuffer[rxBufferPos] = c;

                if (rxBufferPos >= MAX_RX_BUFFER - 2) {
                        // Bell
                        output->transmit ("\a");
                }
                else {
                        // Echo
                        output->transmit (&c, 1);
                        ++rxBufferPos;
                }
        }
        else {
                rxBuffer[rxBufferPos] = '\0';

                if (++rxBufferPos >= MAX_RX_BUFFER) {
                        // Should never happen, because
                        Error_Handler ();
                }

                // Echo
                output->transmit ("\n", 1);

                // Just to be sure, but it has to be configured in NVIC anyway (preemption).
                input->pause ();
                lineBuffer.pushBack ((const char *)rxBuffer);
                input->resume ();

                rxBufferPos = 0;
        }
}

/*****************************************************************************/

void Console::run ()
{
        std::pair<char const *, uint8_t> line;
        while (!lineBuffer.isEmpty ()) {
                line = lineBuffer.front ();
                onNewLine (line.first, line.second);
                output->transmit ("$", 1);

                input->pause ();
                lineBuffer.popFront ();
                input->resume ();
        }
}

/*****************************************************************************/

void Console::onNewLine (const char *str, size_t len) {}
