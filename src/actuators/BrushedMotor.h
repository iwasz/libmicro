/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_BRUSHEDMOTOR_H
#define LIB_MICRO_BRUSHEDMOTOR_H

#include "Pwm.h"
#include <cstdint>

class Gpio;

/**
 * @brief The BrushedMotor class
 */
class BrushedMotor {
public:
        BrushedMotor (Gpio *dir, Pwm *p, Pwm::Channel channel, uint32_t fullScale)
            : direction (dir), pwm (p), channel (channel), factor (fullScale / 100), fullScale (fullScale)
        {
        }

        /**
         * @brief setSpeed
         * @param speed from -100 to 100
         */
        void setSpeed (int32_t speed);

private:
        Gpio *direction;
        Pwm *pwm;
        Pwm::Channel channel;
        uint32_t factor;
        uint32_t fullScale;
};

#endif // BRUSHEDMOTOR_H
