/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "CanFrame.h"
#include "Hal.h"
#include "ICanCallback.h"
#include "stm32f4xx_hal_can.h"
#include <cstdint>

extern "C" void CAN1_RX0_IRQHandler ();
extern "C" void CAN1_RX1_IRQHandler ();
extern "C" void CAN1_TX_IRQHandler ();
extern "C" void CAN1_SCE_IRQHandler ();
/**
 * Tested only on F4s.
 */
class Can {
public:
        Can (ICanCallback *callback = nullptr, uint32_t prescaler = 6, uint32_t sjw = CAN_SJW_3TQ, uint32_t bs1 = CAN_BS1_12TQ,
             uint32_t bs2 = CAN_BS2_3TQ);

        /// Synchronous and blocking
        bool send (CanFrame const &buff, uint32_t timeoutMs = 1000);

        /// Synchronous and blocking
        CanFrame read (uint32_t timeoutMs = 1000);
        void setFilterAndMask (uint32_t filter, uint32_t mask, bool extended);

        /// Asyncronous, callback called from ISR.
        void setCanCallback (ICanCallback *c) { callback = c; }

        /// Turns interrupts on or off. TODO Check if synchronous read works when turned on.
        void interrupts (bool on, bool errors = true);

        /// Reset and put in loopback mode.
        void disable ();

        /// Resets (put in sleep mode).
        void reset ();

        /// Modes of operation as described in reference manual chapter 29.4
        enum Mode { SLEEP, INITIALIZATION, NORMAL };

        /// Sets the mode. Blocks.
        void setMode (Mode mode);

        void setBaudratePrescaler (uint32_t prescaler);

        void setAutomaticRetransmission (bool b);

private:
        friend void HAL_CAN_TxMailbox0CompleteCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_TxMailbox1CompleteCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_TxMailbox2CompleteCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_TxMailbox0AbortCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_TxMailbox1AbortCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_TxMailbox2AbortCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_RxFifo0MsgPendingCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_RxFifo0FullCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_RxFifo1MsgPendingCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_RxFifo1FullCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_SleepCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_WakeUpFromRxMsgCallback (CAN_HandleTypeDef *hcan);
        friend void HAL_CAN_ErrorCallback (CAN_HandleTypeDef *hcan);
        friend void CAN1_RX0_IRQHandler ();
        friend void CAN1_RX1_IRQHandler ();
        friend void CAN1_TX_IRQHandler ();
        friend void CAN1_SCE_IRQHandler ();

        static void clkEnable (CAN_HandleTypeDef *canX);
        static void clkDisable (CAN_HandleTypeDef *spiX);
        void clkEnable () { clkEnable (&canHandle); }
        void clkDisable () { clkDisable (&canHandle); }

        CAN_HandleTypeDef canHandle{};
        CAN_TxHeaderTypeDef txHeader{};
        CAN_RxHeaderTypeDef rxHeader{};
        ICanCallback *callback;
        static Can *can;
};
