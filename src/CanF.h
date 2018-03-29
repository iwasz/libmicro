/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIBMICRO_CAN_H__
#define LIBMICRO_CAN_H__

#include "CanFrame.h"
#include "Hal.h"
#include "ICanCallback.h"
#include <cstdint>

// extern "C" void CEC_CAN_IRQHandler ();

class Can {
public:
        Can (ICanCallback *callback = nullptr, uint32_t timeout = 1000, uint32_t prescaler = 6, uint32_t sjw = CAN_SJW_1TQ,
             uint32_t bs1 = CAN_BS1_13TQ, uint32_t bs2 = CAN_BS2_2TQ);

        bool send (CanFrame const &buff);
        CanFrame read ();
        void setFilterAndMask (uint32_t filter, uint32_t mask, bool extended);
        void setCanCallback (ICanCallback *c) { callback = c; }

private:
        static void clkEnable (CAN_HandleTypeDef *canX);
        static void clkDisable (CAN_HandleTypeDef *spiX);
        void clkEnable () { clkEnable (&canHandle); }
        void clkDisable () { clkDisable (&canHandle); }

private:
        //        friend void CEC_CAN_IRQHandler ();
        CAN_HandleTypeDef canHandle;
        ICanCallback *callback;
        uint32_t sendTimeoutMs;
};

#endif
