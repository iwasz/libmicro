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

#define COUNTS_PER_SECOND 10000
#define SCALE_FACTOR 0.5
//#define MIN_MOTOR_SPEED 10

void BipolarStepper::setSpeed (int32_t speed)
{
        if (speed > 100) {
                speed = 100;
        }
        else if (speed < -100) {
                speed = -100;
        }

        speed = (int)(COUNTS_PER_SECOND * SCALE_FACTOR * 60) / (stepsNo * (speed /*+ MIN_MOTOR_SPEED*/));

        if (speed == this->speed) {
                return;
        }

        this->speed = speed;
        counter = 0;
}

// void BipolarStepper::stepLeft ()
//{
//        //        bPhasePin->set (cycle / 2);
//        //        cycle = uint8_t (cycle + 1) % 4;
//        //        aPhasePin->set (cycle / 2);

//        switch (cycle) {
//        case 0:
//                aPhasePin->set (true);
//                aEnablePin->set (true);

//                // bPhasePin->set (false);
//                bEnablePin->set (false);
//                break;
//        case 1:
//                // aPhasePin->set (true);
//                aEnablePin->set (false);

//                bPhasePin->set (true);
//                bEnablePin->set (true);
//                break;
//        case 2:
//                aPhasePin->set (false);
//                aEnablePin->set (true);

//                // bPhasePin->set (false);
//                bEnablePin->set (false);
//                break;
//        case 3:
//                // aPhasePin->set (true);
//                aEnablePin->set (false);

//                bPhasePin->set (false);
//                bEnablePin->set (true);
//                break;
//        default:
//                break;
//        };

//        cycle = uint8_t (cycle + 1) % 4;
//}

void BipolarStepper::stepLeft ()
{
        //        bPhasePin->set (cycle / 2);
        //        cycle = uint8_t (cycle + 1) % 4;
        //        aPhasePin->set (cycle / 2);
        --cycle;

        if (cycle > 7) {
                cycle = 7;
        }

        switch (cycle) {
        case 0:
                aPhasePin->set (true);
                aEnablePin->set (true);

                // bPhasePin->set (false);
                bEnablePin->set (false);
                break;

        case 1:
                aPhasePin->set (true);
                aEnablePin->set (true);

                bPhasePin->set (true);
                bEnablePin->set (true);
                break;

        case 2:
                // aPhasePin->set (true);
                aEnablePin->set (false);

                bPhasePin->set (true);
                bEnablePin->set (true);
                break;

        case 3:
                aPhasePin->set (false);
                aEnablePin->set (true);

                bPhasePin->set (true);
                bEnablePin->set (true);
                break;

        case 4:
                aPhasePin->set (false);
                aEnablePin->set (true);

                // bPhasePin->set (false);
                bEnablePin->set (false);
                break;

        case 5:
                aPhasePin->set (false);
                aEnablePin->set (true);

                bPhasePin->set (false);
                bEnablePin->set (true);
                break;

        case 6:
                // aPhasePin->set (true);
                aEnablePin->set (false);

                bPhasePin->set (false);
                bEnablePin->set (true);
                break;

        case 7:
                aPhasePin->set (true);
                aEnablePin->set (true);

                bPhasePin->set (false);
                bEnablePin->set (true);
                break;

        default:
                break;
        };
}

void BipolarStepper::stepRight ()
{
        //        aPhasePin->set (cycle / 2);
        //        cycle = uint8_t (cycle + 1) % 4;
        //        bPhasePin->set (cycle / 2);

        cycle = uint8_t (cycle + 1) % 8;

        switch (cycle) {
        case 0:
                aPhasePin->set (true);
                aEnablePin->set (true);

                // bPhasePin->set (false);
                bEnablePin->set (false);
                break;

        case 1:
                aPhasePin->set (true);
                aEnablePin->set (true);

                bPhasePin->set (true);
                bEnablePin->set (true);
                break;

        case 2:
                // aPhasePin->set (true);
                aEnablePin->set (false);

                bPhasePin->set (true);
                bEnablePin->set (true);
                break;

        case 3:
                aPhasePin->set (false);
                aEnablePin->set (true);

                bPhasePin->set (true);
                bEnablePin->set (true);
                break;

        case 4:
                aPhasePin->set (false);
                aEnablePin->set (true);

                // bPhasePin->set (false);
                bEnablePin->set (false);
                break;

        case 5:
                aPhasePin->set (false);
                aEnablePin->set (true);

                bPhasePin->set (false);
                bEnablePin->set (true);
                break;

        case 6:
                // aPhasePin->set (true);
                aEnablePin->set (false);

                bPhasePin->set (false);
                bEnablePin->set (true);
                break;

        case 7:
                aPhasePin->set (true);
                aEnablePin->set (true);

                bPhasePin->set (false);
                bEnablePin->set (true);
                break;

        default:
                break;
        };
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
