/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ICHARACTERSINK_H
#define ICHARACTERSINK_H

#include <cstdint>

struct ICharacterSink {
        virtual ~ICharacterSink () {}
        virtual void onData (char c) = 0;
        virtual void onError (uint32_t flags) = 0;
};

#endif // ICHARACTERSINK_H
