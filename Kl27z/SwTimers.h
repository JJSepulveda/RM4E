#ifndef SWTIMERS_H_
#define SWTIMERS_H_
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Includes Section                                         
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
//                                  Defines & Macros Section                                     
///////////////////////////////////////////////////////////////////////////////////////////////////
#define SWTIMERS_TIME_BASE_HZ	(100) /* time base in hertz */

#define SWTIMER_TIME_BASE_MS	(1000/SWTIMERS_TIME_BASE_HZ)
///////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Typedef Section                                          
///////////////////////////////////////////////////////////////////////////////////////////////////

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

void vfnSwTimers_Init(void);

uint8_t bfnSwTimers_RequestTimer(void);

void vfnSwTimers_StartTimer(uint8_t Channel, uint16_t Timeout);

uint8_t bfnSwTimers_GetStatus(uint8_t Channel);

void vfnSwTimers_Task(void);

#if defined(__cplusplus)
}
#endif // __cplusplus


#endif /* SWTIMERS_H_ */
///////////////////////////////////////////////////////////////////////////////////////////////////
// EOF
///////////////////////////////////////////////////////////////////////////////////////////////////
