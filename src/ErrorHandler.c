/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ErrorHandler.h"
#ifdef UNIT_TEST
#include <cstring>
#include <iostream>
#endif

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void Error_Handler (void)
{
#ifdef UNIT_TEST
        std::cerr << strerror (errno) << std::endl;
#endif

        while (1) {
        }
}
