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
 */
class BipolarStepper {
public:
        BipolarStepper (Gpio *ap, Gpio *ae, Gpio *bp, Gpio *be, uint16_t s)
            : aPhasePin (ap), aEnablePin (ae), bPhasePin (bp), bEnablePin (be), stepsNo (s), cycle (0)
        {
        }

        /**
         * @brief setSpeed
         * @param speed from -100 to 100
         */
        void setSpeed (int32_t speed);

//private:

        void power (bool on);

        /// Move n steps (- or +).
        void step (int n);

private:
        Gpio *aPhasePin;
        Gpio *aEnablePin;
        Gpio *bPhasePin;
        Gpio *bEnablePin;
        uint16_t stepsNo;
        uint8_t cycle;
};

#endif // BIPOLARSTEPPER_H
