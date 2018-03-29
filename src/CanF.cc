/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Can.h"
#include "Types.h"
#include <Debug.h>
#include <ErrorHandler.h>
#include <cstdlib>
#include <cstring>

// C-CAN callbacks
// extern "C" {
// void CEC_CAN_IRQHandler (void) { HAL_CAN_IRQHandler (&CanDriver::canHandle); }

// void HAL_CAN_RxCpltCallback (CAN_HandleTypeDef *hcan)
//{
//        Debug::singleton ()->print ("s:");
//        Debug::singleton ()->print (to_ascii ((uint8_t *)&hcan->pRxMsg->StdId, 4).c_str ());
//        Debug::singleton ()->print (", e:");
//        Debug::singleton ()->print (to_ascii ((uint8_t *)&hcan->pRxMsg->ExtId, 4).c_str ());
//        Debug::singleton ()->print (", data:");
//        Debug::singleton ()->print (to_ascii (hcan->pRxMsg->Data, hcan->pRxMsg->DLC).c_str ());
//        Debug::singleton ()->print ("\n");
//        CanDriver::instance ()->read (NULL);
//}
//} // extern "C"

Can::Can (ICanCallback *callback, uint32_t timeout, uint32_t prescaler, uint32_t sjw, uint32_t bs1, uint32_t bs2)
    : callback (callback), sendTimeoutMs (timeout)
{
        clkEnable ();
        canHandle.Instance = CAN;

        static CanTxMsgTypeDef txMessage;
        static CanRxMsgTypeDef rxMessage;
        canHandle.pTxMsg = &txMessage;
        canHandle.pRxMsg = &rxMessage;

        canHandle.Init.TTCM = DISABLE;
        canHandle.Init.ABOM = DISABLE;
        canHandle.Init.AWUM = DISABLE;
        canHandle.Init.NART = DISABLE;
        canHandle.Init.RFLM = DISABLE;
        canHandle.Init.TXFP = DISABLE;
        canHandle.Init.Mode = CAN_MODE_NORMAL;
        canHandle.Init.Prescaler = prescaler;
        //        canHandle.Init.Prescaler = 12; // 250kbps
        canHandle.Init.SJW = sjw;
        canHandle.Init.BS1 = bs1;
        canHandle.Init.BS2 = bs2;

        if (HAL_CAN_Init (&canHandle) != HAL_OK) {
                Error_Handler ();
        }

                // To pomagało przy monitorAll
#if 0
        CAN_FilterConfTypeDef sFilterConfig;
        sFilterConfig.FilterNumber = 0;
        sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
        sFilterConfig.FilterIdHigh = 0x0000;
        sFilterConfig.FilterIdLow = 0x0000;
        sFilterConfig.FilterMaskIdHigh = 0x0000;
        sFilterConfig.FilterMaskIdLow = 0x0000;
        sFilterConfig.FilterFIFOAssignment = 0;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 0;

        if (HAL_CAN_ConfigFilter (&canHandle, &sFilterConfig) != HAL_OK) {
                Error_Handler ();
        }

        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_FMP0);
        __HAL_CAN_ENABLE_IT (&canHandle, CAN_IT_FMP1);
#endif
}

/**
 * Transmits a sequence of bytes to the ECU over CAN bus
 * @parameter   buff   CanMsgBuffer instance
 * @return the send operation completion status
 */
bool Can::send (const CanFrame &buff)
{
        *canHandle.pTxMsg = buff.toNative ();

        // Sprawdzenie czy można wysłać jest w środku
        if (HAL_CAN_Transmit (&canHandle, sendTimeoutMs) != HAL_OK) {
                if (callback) {
                        callback->onCanError (ICanCallback::SEND_TIMEOUT);
                }

                return false;
        }

        return true;
}

/**
 * Set the CAN filter for FIFO buffer
 * @parameter   filter    CAN filter value
 * @parameter   mask      CAN mask value
 * @parameter   extended  CAN extended message flag
 * @return  the operation completion status
 */
void Can::setFilterAndMask (uint32_t filter, uint32_t mask, bool extended)
{
        CAN_FilterConfTypeDef sFilterConfig;
        sFilterConfig.FilterNumber = 0;
        sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
        sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

        if (extended) {
                filter <<= 3;
                mask <<= 3;
                filter |= 0x04;
                mask |= 0x04;

                sFilterConfig.FilterIdHigh = filter >> 16;
                sFilterConfig.FilterIdLow = filter & 0x0000ffff;
                sFilterConfig.FilterMaskIdHigh = mask >> 16;
                sFilterConfig.FilterMaskIdLow = mask & 0x0000ffff;
        }
        else {
                filter <<= 21;
                mask <<= 21;

                sFilterConfig.FilterIdHigh = filter >> 16;
                sFilterConfig.FilterIdLow = filter & 0x0000ffff;
                sFilterConfig.FilterMaskIdHigh = mask >> 16;
                sFilterConfig.FilterMaskIdLow = mask & 0x0000ffff;
        }

        sFilterConfig.FilterFIFOAssignment = 0;
        sFilterConfig.FilterActivation = ENABLE;
        sFilterConfig.BankNumber = 0;

        if (HAL_CAN_ConfigFilter (&canHandle, &sFilterConfig) != HAL_OK) {
                Error_Handler ();
        }
}

/**
 * Read the CAN frame from FIFO buffer
 * @return  true if read the frame / false if no frame
 */
CanFrame Can::read ()
{
        // Z tym działa monotoring.
        if (HAL_CAN_Receive_IT (&canHandle, CAN_FIFO0) != HAL_OK) {
                if (callback) {
                        callback->onCanError (ICanCallback::READ_ERROR);
                }

                return CanFrame ();
        }

        //        if (HAL_CAN_Receive (&canHandle, CAN_FIFO0, CAN_READ_TIMEOUT) != HAL_OK) {
        //                // TODO inaczej obsługa błedów, opcja ErrorHandler, który nie blokuje
        //                // Error_Handler ();
        //                return CanFrame ();
        //        }

        return CanFrame (*canHandle.pRxMsg);
}

/*****************************************************************************/

void Can::clkEnable (CAN_HandleTypeDef *canX)
{
        if (canX->Instance == CAN) {
                __HAL_RCC_CAN1_CLK_ENABLE ();
        }
}

/*****************************************************************************/

void Can::clkDisable (CAN_HandleTypeDef *canX)
{
        if (canX->Instance == CAN) {
                __HAL_RCC_CAN1_CLK_DISABLE ();
        }
}
