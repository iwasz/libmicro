/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef I2CCALLBACK_H
#define I2CCALLBACK_H

struct II2cCallback {
        virtual ~II2cCallback () {}
        virtual void onRxComplete (/*const uint8_t *data, size_t size*/) = 0;
};

#endif // I2CCALLBACK_H
