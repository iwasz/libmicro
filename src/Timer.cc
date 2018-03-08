/*
 *          _,.-------.,_
 *      ,;~'             '~;,
 *    ,;                     ;,
 *   ;                         ;
 *  ,'                         ',
 * ,;                           ;,
 * ; ;      .           .      ; ;
 * | ;   ______       ______   ; |
 * |  `/~"     ~" . "~     "~\'  |
 * |  ~  ,-~~~^~, | ,~^~~~-,  ~  |
 *  |   |        }:{        |   |
 *  |   l       / | \       !   |
 *  .~  (__,.--" .^. "--.,__)  ~.
 *  |     ---;' / | \ `;---     |
 *   \__.       \/^\/       .__/
 *    V| \                 / |V
 *     | |T~\___!___!___/~T| |
 *     | |`IIII_I_I_I_IIII'| |
 *     |  \,III I I I III,/  |
 *      \   `~~~~~~~~~~'    /
 *        \   .       .   /     -dcau (4/15/95)
 *          \.    ^    ./
 *            ^~~~^~~~^
 *
 *
 */

#include "Timer.h"
#include "Hal.h"
#include <cstdint>

/*****************************************************************************/

Timer::Timer () : startTime (0), intervalMs (0) {}

/*****************************************************************************/

void Timer::start (uint32_t interval)
{
        startTime = HAL_GetTick ();
        this->intervalMs = interval;
}

/*****************************************************************************/

bool Timer::isExpired () const { return HAL_GetTick () - startTime >= intervalMs; }

