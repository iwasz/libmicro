/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_I2C_COMMON_FILE_H
#define LIB_MICRO_I2C_COMMON_FILE_H

#include "Hal.h"

#if defined(LIB_MICRO_STM32F)
#include "I2cF.h"
#elif defined(LIB_MICRO_NRG)
#include "I2cNRG.h"
#endif

#endif // I2C_H
