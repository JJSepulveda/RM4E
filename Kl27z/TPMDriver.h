/*
 * TPMDriver.h
 *
 *  Created on: Apr 9, 2018
 *      Author: JJ
 */

#ifndef TPMDRIVER_H_
#define TPMDRIVER_H_
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "MKL27Z644.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Defines & Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#define TPMDIVER_INPUT_CAPTURE          (1 << TPM_CnSC_ELSB_SHIFT)

#define TPMDIVER_EDGE_ALIGNED_PWM       ((1 << TPM_CnSC_MSB_SHIFT) | (1 << TPM_CnSC_ELSB_SHIFT))

#define TPMDRIVER_DISABLE_MODE			(0)

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Typedef Section
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TPM_select_module_t
{
    _TPM0_M = 0,
    _TPM1_M,
    _TPM2_M,
    MAX_TPM_MODULES
};

typedef void (* TPMFuncionQueSeLlamaEnLaInterrupcion_t) (uint8_t );
typedef void (* TPMFuncionQueSeLlamaEnLaInterrupcionDeOverflow_t) (void);
typedef void (* TPMFuncionQueSeLlamaEnLaInterrupcionDeCounterChannel_t) (uint8_t);

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                Function-like Macros Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Extern Constants Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Extern Variables Section
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                Function Prototypes Section
///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

void vfnTPMDriver_InitTPM(uint8_t b_TPM, uint16_t wModule, uint8_t bPrescale);

void vfnTPMDriver_Mode(uint8_t bModuleOfTPM, uint8_t bChannel, uint8_t bModoDeOperacionDelTPM,uint8_t bPortNumber,uint8_t bPortLetter,uint8_t bActivate);

void vfnTPMDriver_StartInputCapture(uint8_t bModuleOfTPM,uint8_t bChannel, TPMFuncionQueSeLlamaEnLaInterrupcion_t wfnCallback, TPMFuncionQueSeLlamaEnLaInterrupcionDeOverflow_t vfnCallBackOverflow);

void vfnTPMDriver_StopInputCaputre(uint8_t bModuleOfTPM,uint8_t bChannel);

void vfnTPMDriver_RestartInputCapture(uint8_t bModuleOfTPM,uint8_t bChannel);

void vfnTPMDriver_SetTimeWork(uint8_t bModuleOfTPM,uint8_t bChannel, uint16_t wTimeWork);

void vfnTPMDriver_ChangeTimeWork(uint8_t bTPM_module, uint8_t bchannel,uint16_t wChangeTimeWork);

uint16_t wfnTPMDriver_GetWimeWork(uint8_t bTPM_module, uint8_t bChannel);

void vfnTPMDriver_chageFrequency(uint8_t bTPM_module, uint16_t bfrequency);

uint16_t wfnTPMDriver_getFrequency(void);

uint16_t wfnTPMDriver_getModuleConf(void);

void vfnTPMDriver_ActivateChannelInterrupt(uint8_t bnumberOfTPM, uint8_t btpmChannel, uint8_t bMotorNumber);

void vfnTPMDriver_setOverFlowCallBack (TPMFuncionQueSeLlamaEnLaInterrupcionDeOverflow_t vfnCallback);

void vfnTPMDriver_setCounterChannelCallBack (TPMFuncionQueSeLlamaEnLaInterrupcionDeCounterChannel_t vfnCallback);

void vnfTPMDriver_StartTPMCounter(uint8_t bTPMNumber, uint8_t bStart);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif /* TPMDRIVER_H_ */
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
