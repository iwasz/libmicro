/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "catch.hpp"
#include "collection/CharacterCircularQueue.h"
#include <iostream>
#include <string>

/*****************************************************************************/

/**
 * @brief
 */
TEST_CASE ("push", "[ccq2]")
{
        CharacterCircularQueue<16> buf;
        buf.pushBack ("ala");
        REQUIRE (buf.front ().first == std::string ("ala"));

        buf.pushBack ("ma");
        REQUIRE (buf.front ().first == std::string ("ala"));
}

/**
 * @brief
 */
TEST_CASE ("push+pop", "[ccq2]")
{
        CharacterCircularQueue<16> buf;

        /* io
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("ala"));

        /*  o         i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |4|a|l|a|0| | | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ala"));
        REQUIRE (buf.front ().second == 3);
        REQUIRE (buf.pushBack ("ma"));

        /*  o                 i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |4|a|l|a|0|3|m|a|0| | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ala"));
        REQUIRE (buf.popFront ());

        /*            o       i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | |3|m|a|0| | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ma"));
        REQUIRE (buf.pushBack ("kota"));

        /*            o                   i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | |3|m|a|0|5|k|o|t|a|0| |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("ma"));
        REQUIRE (buf.popFront ());

        /*                    o           i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |5|k|o|t|a|0| |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("kota"));
        REQUIRE (buf.pushBack ("oraz"));

        /*              i     o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0| | | |5|k|o|t|a|0|E|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("kota"));
        REQUIRE (buf.popFront ());

        /*  o           i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0| | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (buf.pushBack ("psaA"));

        /*  o                       i
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0|5|p|s|a|A|0| | | | |
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (!buf.pushBack ("abc"));
        REQUIRE (buf.pushBack ("ab"));

        /* io
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * |5|o|r|a|z|0|5|p|s|a|A|0|3|a|b|0|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("oraz"));
        REQUIRE (!buf.pushBack ("ab"));
        REQUIRE (buf.popFront ());

        /*  i           o
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         * | | | | | | |5|p|s|a|A|0|3|a|b|0|
         * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("psaA"));
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push+pop2", "[ccq2]")
{
        CharacterCircularQueue<9> buf;

        REQUIRE (buf.isEmpty ());
        REQUIRE (buf.pushBack ("a"));
        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.pushBack ("b"));
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.pushBack ("c"));
        REQUIRE (buf.getSize () == 3);
        REQUIRE (!buf.pushBack ("d"));

        REQUIRE (buf.front ().first == std::string ("a"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.front ().first == std::string ("b"));
        REQUIRE (buf.pushBack ("d"));
        REQUIRE (buf.getSize () == 3);

        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 2);
        REQUIRE (buf.front ().first == std::string ("c"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 1);
        REQUIRE (buf.front ().first == std::string ("d"));
        REQUIRE (buf.popFront ());
        REQUIRE (buf.getSize () == 0);
        REQUIRE (buf.isEmpty ());
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push to much", "[ccq2]")
{
        CharacterCircularQueue<9> buf;

        // Push so much, that all the space in buffer is used.
        REQUIRE (buf.pushBack ("1234567"));
        REQUIRE (buf.popFront ());
        REQUIRE (!buf.pushBack ("12345678"));
}

/**
 * @brief TEST_CASE
 */
TEST_CASE ("push and remove at once", "[ccq2]")
{
        CharacterCircularQueue<9> buf;

        /* io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("12"));

        /*  o       i
         * +-+-+-+-+-+-+-+-+-+
         * |3|1|2|0| | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("12"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        /*         io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("34"));

        /*          o       i
         * +-+-+-+-+-+-+-+-+-+
         * | | | | |2|3|4|0| |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.front ().first == std::string ("34"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        /*                 io
         * +-+-+-+-+-+-+-+-+-+
         * | | | | | | | | | |
         * +-+-+-+-+-+-+-+-+-+
         */

        REQUIRE (buf.pushBack ("45"));
        REQUIRE (buf.front ().first == std::string ("45"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("67"));
        REQUIRE (buf.front ().first == std::string ("67"));
        REQUIRE (buf.front ().second == 2);
        REQUIRE (buf.popFront ());
}

TEST_CASE ("push and remove instantly", "[ccq2]")
{
        CharacterCircularQueue<256> buf;

        // Push so much, that all the space in buffer is used.
        REQUIRE (buf.pushBack ("111111111111111111111111111111111111111111111111111111111111111111111111"));
        REQUIRE (buf.front ().first == std::string ("111111111111111111111111111111111111111111111111111111111111111111111111"));
        REQUIRE (buf.front ().second == 72);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("22222222222222222222222222222222222222222222222222222222222222222222222222"));
        REQUIRE (buf.front ().first == std::string ("22222222222222222222222222222222222222222222222222222222222222222222222222"));
        REQUIRE (buf.front ().second == 74);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("3333333333333333333333333333333333333333333333333333333333333333333333333333333"));
        REQUIRE (buf.front ().first == std::string ("3333333333333333333333333333333333333333333333333333333333333333333333333333333"));
        REQUIRE (buf.front ().second == 79);
        REQUIRE (buf.popFront ());

        REQUIRE (buf.pushBack ("44444444444444444444444444444444444444444444444444444444444444444444444444444444"));
        REQUIRE (buf.front ().first == std::string ("44444444444444444444444444444444444444444444444444444444444444444444444444444444"));
        REQUIRE (buf.front ().second == 80);
        REQUIRE (buf.popFront ());
}

// Testy funkcji ramp do silnika krokowego
#if 0

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/*! \brief Holding data used by timer interrupt for speed ramp calculation.
 *
 *  Contains data used by timer interrupt to calculate speed profile.
 *  Data is written to it by move(), when stepper motor is moving (timer
 *  interrupt running) data is read/updated when calculating a new step_delay
 */
typedef struct {
        //! What part of the speed ramp we are in.
        unsigned char run_state : 3;
        //! Direction stepper motor should move.
        unsigned char dir : 1;
        //! Peroid of next timer delay. At start this value set the accelration rate.
        unsigned int step_delay;
        //! What step_pos to start decelaration
        unsigned int decel_start;
        //! Sets deceleration rate.
        signed int decel_val;
        //! Minimum time delay (max speed)
        signed int min_delay;
        //! Counter used when accelerateing/decelerateing to calculate step_delay.
        signed int accel_count;
} speedRampData;

// Direction of stepper motor movement
enum Direction { CW, CCW };

/*! \Brief Frequency of timer1 in [Hz].
 *
 * Modify this according to frequency used. Because of the prescaler setting,
 * the timer1 frequency is the clock frequency divided by 8.
 */
// Timer/Counter 1 running on 3,686MHz / 8 = 460,75kHz (2,17uS). (T1-FREQ 460750)
#define T1_FREQ 460750

//! Number of (full)steps per round on stepper motor in use.
#define FSPR 200

#define FULLSTEPS

#ifdef HALFSTEPS
#define SPR (FSPR * 2)
#endif
#ifdef FULLSTEPS
#define SPR FSPR
#endif

#ifndef HALFSTEPS
#ifndef FULLSTEPS
#error FULLSTEPS/HALFSTEPS not defined!
#endif
#endif

// Maths constants. To simplify maths when calculating in speed_cntr_Move().
#define ALPHA (2 * 3.14159 / SPR)                    // 2*pi/spr
#define A_T_x100 ((long)(ALPHA * T1_FREQ * 100))     // (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int)((T1_FREQ * 0.676) / 100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA * 2 * 10000000000)         // ALPHA*2*10000000000
#define A_x20000 (int)(ALPHA * 20000)                // ALPHA*20000

// Speed ramp states
#define STOP 0
#define ACCEL 1
#define DECEL 2
#define RUN 3

/*! \brief Status flags
 */
struct GLOBAL_FLAGS {
        //! True when stepper motor is running.
        unsigned char running : 1;
        //! True when uart has received a string (ended with '/r').
        unsigned char cmd : 1;
        //! Dummy bits to fill up a byte.
        unsigned char dummy : 6;
};

struct GLOBAL_FLAGS status;

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

//! Cointains data for timer interrupt.
speedRampData srd;

class SpeedController {
public:
        SpeedController () { srd.run_state = STOP; }

        /**
         * \brief Move the stepper motor a given number of steps.
         *
         *  Makes the stepper motor move the given number of steps.
         *  It accelrate with given accelration up to maximum speed and decelerate
         *  with given deceleration so it stops at the given step.
         *  If accel/decel is to small and steps to move is to few, speed might not
         *  reach the max speed limit before deceleration starts.
         *
         *  \param step  Number of steps to move (pos - CW, neg - CCW).
         *  \param accel  Accelration to use, in 0.01*rad/sec^2.
         *  \param decel  Decelration to use, in 0.01*rad/sec^2.
         *  \param speed  Max speed, in 0.01*rad/sec.
         */
        void move (signed int step, unsigned int accel, unsigned int decel, unsigned int speed);

        /**
         * Has to be run at each timer event.
         */
        void timeStep ();
};

void SpeedController::move (signed int step, unsigned int accel, unsigned int decel, unsigned int speed)
{
        //! Number of steps before we hit max speed.
        unsigned int max_s_lim;
        //! Number of steps before we must start deceleration (if accel does not hit max speed).
        unsigned int accel_lim;

        // Set direction from sign on step value.
        if (step < 0) {
                srd.dir = CCW;
                step = -step;
        }
        else {
                srd.dir = CW;
        }

        // If moving only 1 step.
        if (step == 1) {
                // Move one step...
                srd.accel_count = -1;
                // ...in DECEL state.
                srd.run_state = DECEL;
                // Just a short delay so main() can act on 'running'.
                srd.step_delay = 1000;
                status.running = true;

                // OCR1A = 10;
                // Run Timer/Counter 1 with prescaler = 8.
                // TCCR1B |= ((0 << CS12) | (1 << CS11) | (0 << CS10));
        }
        // Only move if number of steps to move is not zero.
        else if (step != 0) {
                // Refer to documentation for detailed information about these calculations.

                // Set max speed limit, by calc min_delay to use in timer.
                // min_delay = (alpha / tt)/ w
                srd.min_delay = A_T_x100 / speed;

                // Set accelration by calc the first (c0) step delay .
                // step_delay = 1/tt * sqrt(2*alpha/accel)
                // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000

                // TODO można użyć tej magicnzje lub wbudowanej SQRT
                srd.step_delay = (T1_FREQ_148 * sqrt (A_SQ / accel)) / 100;

                // Find out after how many steps does the speed hit the max speed limit.
                // max_s_lim = speed^2 / (2*alpha*accel)
                max_s_lim = (long)speed * speed / (long)(((long)A_x20000 * accel) / 100);
                // If we hit max speed limit before 0,5 step it will round to 0.
                // But in practice we need to move atleast 1 step to get any speed at all.
                if (max_s_lim == 0) {
                        max_s_lim = 1;
                }

                // Find out after how many steps we must start deceleration.
                // n1 = (n1+n2)decel / (accel + decel)
                accel_lim = ((long)step * decel) / (accel + decel);
                // We must accelrate at least 1 step before we can start deceleration.
                if (accel_lim == 0) {
                        accel_lim = 1;
                }

                // Use the limit we hit first to calc decel.
                if (accel_lim <= max_s_lim) {
                        srd.decel_val = accel_lim - step;
                }
                else {
                        srd.decel_val = -((long)max_s_lim * accel) / decel;
                }
                // We must decelrate at least 1 step to stop.
                if (srd.decel_val == 0) {
                        srd.decel_val = -1;
                }

                // Find step to start decleration.
                srd.decel_start = step + srd.decel_val;

                // If the maximum speed is so low that we dont need to go via accelration state.
                if (srd.step_delay <= srd.min_delay) {
                        srd.step_delay = srd.min_delay;
                        srd.run_state = RUN;
                }
                else {
                        srd.run_state = ACCEL;
                }

                // Reset counter.
                srd.accel_count = 0;
                status.running = true;

                // OCR1A = 10;
                // Set Timer/Counter to divide clock by 8
                // TCCR1B |= ((0 << CS12) | (1 << CS11) | (0 << CS10));
        }
}

/*****************************************************************************/

void SpeedController::timeStep ()
{
        // Holds next delay period.
        unsigned int new_step_delay;
        // Remember the last step delay used when accelrating.
        static int last_accel_delay;
        // Counting steps when moving.
        static unsigned int step_count = 0;
        // Keep track of remainder from new_step-delay calculation to incrase accurancy
        static unsigned int rest = 0;

        // TODO USTAW opóźnienie timera (może ustaw ARR).
        // OCR1A = srd.step_delay;
        std::cout << "Ustaw delay timera na : " << srd.step_delay << std::endl;

        switch (srd.run_state) {
        case STOP:
                step_count = 0;
                rest = 0;

                // TODO Stop Timer/Counter 1.
                // TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));

                status.running = false;
                break;

        case ACCEL:
                // TODO Wyślij sygnał do drivera
                // sm_driver_StepCounter (srd.dir);

                step_count++;
                srd.accel_count++;
                new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest) / (4 * srd.accel_count + 1));
                rest = ((2 * (long)srd.step_delay) + rest) % (4 * srd.accel_count + 1);

                // Check if we should start decelration.
                if (step_count >= srd.decel_start) {
                        srd.accel_count = srd.decel_val;
                        srd.run_state = DECEL;
                }
                // Chech if we hitted max speed.
                else if (new_step_delay <= srd.min_delay) {
                        last_accel_delay = new_step_delay;
                        new_step_delay = srd.min_delay;
                        rest = 0;
                        srd.run_state = RUN;
                }
                break;

        case RUN:
                // TODO Wyślij sygnał do drivera
                // sm_driver_StepCounter (srd.dir);

                step_count++;
                new_step_delay = srd.min_delay;

                // Check if we should start decelration.
                if (step_count >= srd.decel_start) {
                        srd.accel_count = srd.decel_val;
                        // Start decelration with same delay as accel ended with.
                        new_step_delay = last_accel_delay;
                        srd.run_state = DECEL;
                }
                break;

        case DECEL:
                // TODO Wyślij sygnał do drivera
                // sm_driver_StepCounter (srd.dir);

                step_count++;
                srd.accel_count++;
                new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest) / (4 * srd.accel_count + 1));
                rest = ((2 * (long)srd.step_delay) + rest) % (4 * srd.accel_count + 1);
                // Check if we at last step
                if (srd.accel_count >= 0) {
                        srd.run_state = STOP;
                }
                break;
        }

        srd.step_delay = new_step_delay;
}

TEST_CASE ("Move the fuck from here", "[ccq2]")
{

        SpeedController sc;
        sc.move (1000, 10000, 10000, 500);

        for (int i = 0; i < 1000; ++i) {
                std::cout << i << " ";
                sc.timeStep ();
        }
}
#endif
