/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "BipolarStepper.h"
#include "Gpio.h"
#include <algorithm>

void BipolarStepper::setSpeed (int32_t speed)
{
        //        int newDuty = speed * factor;
        //        newDuty = std::min<int> (std::abs (newDuty), fullScale);
        //        pwm->setDuty (channel, newDuty);
        //        direction->set (speed > 0);
}

/// Move n steps (- or +).
void BipolarStepper::step (int n)
{
        bool dir = n > 0;

        for (int i = 0; i < n; ++i) {
                cycle = (cycle + 1) % 4;

                switch (cycle) {
                case 0:
                        aPhasePin->set (true);
                        bPhasePin->set (false);
                        break;
                case 1:
                        aPhasePin->set (true);
                        bPhasePin->set (true);
                        break;
                case 2:
                        aPhasePin->set (false);
                        bPhasePin->set (true);
                        break;
                case 3:
                        aPhasePin->set (false);
                        bPhasePin->set (false);
                        break;
                default:
                        break;
                }

                HAL_Delay (1);
        }
}

void BipolarStepper::power (bool on)
{
        aEnablePin->set (on);
        bEnablePin->set (on);
}
