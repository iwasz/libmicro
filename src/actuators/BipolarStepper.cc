/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "BipolarStepper.h"
#include "Debug.h"
#include "Gpio.h"
#include <algorithm>
#include <cmath>

void BipolarStepper::timeStep ()
{
        if (speed) {
                if (static_cast<uint32_t> (std::abs (speed)) == counter++) {
                        counter = 0;
                        if (speed > 0) {
                                stepLeft ();
                        }
                        else {
                                stepRight ();
                        }
                }
        }
}

void BipolarStepper::setSpeed (int32_t speed)
{
        this->speed = speed;
        counter = 0;
}

void BipolarStepper::stepLeft ()
{
        bPhasePin->set (cycle / 2);
        cycle = uint8_t (cycle + 1) % 4;
        aPhasePin->set (cycle / 2);
}

void BipolarStepper::stepRight ()
{
        aPhasePin->set (cycle / 2);
        cycle = uint8_t (cycle + 1) % 4;
        bPhasePin->set (cycle / 2);
}

///// Move n steps (- or +).
// void BipolarStepper::step (int n)
//{
//        if (n > 0) {
//                n = std::abs (n);
//                for (int i = 0; i < n; ++i) {
//                        bPhasePin->set (cycle / 2);
//                        cycle = uint8_t (cycle + 1) % 4;
//                        aPhasePin->set (cycle / 2);
//                        //                        HAL_Delay (1);
//                }
//        }
//        else {
//                n = std::abs (n);
//                for (int i = 0; i < n; ++i) {
//                        aPhasePin->set (cycle / 2);
//                        cycle = uint8_t (cycle + 1) % 4;
//                        bPhasePin->set (cycle / 2);
//                        //                        HAL_Delay (1);
//                }
//        }
//}

void BipolarStepper::power (bool on)
{
        aEnablePin->set (on);
        bEnablePin->set (on);
}
