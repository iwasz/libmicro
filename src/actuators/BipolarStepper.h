/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MICRO_BIPOLARSTEPPER_H
#define LIB_MICRO_BIPOLARSTEPPER_H

#include <cstdint>

class Gpio;

/**
 * Full step.
 * TODO
 * - ramp profile(s)
 * - n steps movement
 * - microstepping (someday)
 * - setSpeed to act lineary, from -100 to 100.
 *
 */
class BipolarStepper {
public:
        BipolarStepper (Gpio *ap, Gpio *ae, Gpio *bp, Gpio *be, uint16_t steps)
            : aPhasePin (ap), aEnablePin (ae), bPhasePin (bp), bEnablePin (be), stepsNo (steps), cycle (0), speed (0), steps (0), counter (0)
        {
        }

        void setSpeed (int32_t speed);

        void timeStep ();
        // private:

        void power (bool on);

        /// Move n steps (- or +).
        // void step (int n);

        void stepLeft ();
        void stepRight ();

private:
        Gpio *aPhasePin;
        Gpio *aEnablePin;
        Gpio *bPhasePin;
        Gpio *bEnablePin;
        uint16_t stepsNo;
        uint8_t cycle;
        int speed;
        int steps;
        uint32_t counter;
};

#endif // BIPOLARSTEPPER_H
