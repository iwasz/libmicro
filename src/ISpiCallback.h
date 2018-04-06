/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ISPICALLBACK_H
#define ISPICALLBACK_H

#include <cstdint>

struct ISpiCallback {
        virtual ~ISpiCallback () {}
        virtual void onRxNotEmpty (uint8_t b) = 0;
        // virtual void onTxEmpty () = 0;
        virtual void onSpiError (uint32_t) = 0;
};

#endif // ISPICALLBACK_H
