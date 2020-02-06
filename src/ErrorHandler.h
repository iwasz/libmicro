/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef GP8_STOPWATCH_UTILS_H
#define GP8_STOPWATCH_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {

static const int LAYER_MASK = 0xff000000;
static const int LIBMICRO_LIBRARY_CODE = 0x0000000;

enum { GPIO_ON_TOGGLE = LAYER_MASK & LIBMICRO_LIBRARY_CODE,
       USART_INIT,
       USART_PARITY_ERROR,
       USART_FRAMING_ERROR,
       USART_NOISE,
       BUFFERED_CHARACTER_SINK,
       BUFFERED_CHARACTER_SINK_FATAL_IF_FULL,
       LINE_SINK_ON_ERROR,
       LINE_SINK_FULL,
       LINE_SINK_FULL_2,
       UNKNOWN,
};

#endif

static const char *LIBMICRO_LIBRARY_EXCEPTION[] = {"GPIO_ON_TOGGLE",
                                                   "USART_INIT",
                                                   "USART_PARITY_ERROR",
                                                   "USART_FRAMING_ERROR",
                                                   "USART_NOISE",
                                                   "BUFFERED_CHARACTER_SINK",
                                                   "BUFFERED_CHARACTER_SINK_FATAL_IF_FULL",
                                                   "LINE_SINK_ON_ERROR",
                                                   "LINE_SINK_FULL",
                                                   "LINE_SINK_FULL_2",
                                                   "UNKNOWN"};

__attribute__ ((weak)) void Error_Handler (int code);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H
