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

#include "Hal.h"
#include "Timer.h"
#include <chrono>
#include <cstdint>
#include <iostream>

/*****************************************************************************/

Timer::Timer () : startTime (0), intervalMs (0) {}

/*****************************************************************************/

void Timer::start (uint32_t interval)
{
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now ();
        auto duration = now.time_since_epoch ();
        startTime = std::chrono::duration_cast<std::chrono::milliseconds> (duration).count ();
        // std::cerr << "Start Time : " << startTime << std::endl;
        this->intervalMs = interval;
}

/*****************************************************************************/

bool Timer::isExpired () const { return elapsed () >= intervalMs; }

/*****************************************************************************/

uint32_t Timer::elapsed () const
{
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now ();
        auto duration = now.time_since_epoch ();
        int actualTime = std::chrono::duration_cast<std::chrono::milliseconds> (duration).count ();
        // std::cerr << "Actual : " << actualTime << ", diff : " << actualTime - startTime << ", intervalMs: " << intervalMs << std::endl;
        return actualTime - startTime;
}
