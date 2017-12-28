/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef ICHARACTERSINK_H
#define ICHARACTERSINK_H

struct ICharacterSink {
        virtual ~ICharacterSink () {}
        virtual void onData (char c) = 0;
};

#endif // ICHARACTERSINK_H
