#ifndef GPT_H_
#define GPT_H_

#include "GPT_Private.h"

#define TIM2 ((TIM2Reg*)0x40000000)

void GPT_Init(void);

void GPT_StartTimer(unsigned long int OverFlowTicks);

unsigned char GPT_CheckTimeIsElapsed(void);

unsigned long int GPT_GetElapsedTime(void);

unsigned long int GPT_GetRemainingTime(void);




#endif /* GPT_H_ */
