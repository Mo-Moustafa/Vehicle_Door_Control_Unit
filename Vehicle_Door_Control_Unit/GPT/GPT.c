#include "GPT.h"
#include "Rcc.h"
#include "Std_Types.h"
#include "Bit_Operations.h"

uint8 timerStarted = 0;

/*
 * Function where timer is initialized through:
 * - Enabling RCC clock of TIM2
 * - Adjusting Prescalar to be set that the counter ticks each 1mS
 */
void GPT_Init(void){

	// Enable Timer Clock
	Rcc_Enable(RCC_TIM2);

	// Set the Prescalar and ARR
	TIM2->PSC = 4.7e3 - 1; // 4000000 HZ/4000 = 1000 HZ ~= 1 mS delay

	TIM2->EGR |= 1<< 0;
}


/*
 * Function that takes the OverFlowTicks which are measured in milliSeconds
 * and sets the ARR value based on this Ticks
 * then Enable the timer by setting bit 0 of CR1 Register
 */
void GPT_StartTimer(unsigned long int OverFlowTicks){

	if (!timerStarted){
		timerStarted = 1;
	}

	CLEAR_BIT(TIM2->SR, 0);
	TIM2->CNT = 0;

	TIM2->ARR = OverFlowTicks - 1; // Max ARR Value at which the timer overflows and stops

	//Enable the timer
	SET_BIT(TIM2->CR1, 0);
}


/*
 * A function to return (1) if an overflow occurred after the last call of GPT_StartTimer
 * returns(0) if no overflow occurred or GPT_StartTimer is not called from the last read.
 */
unsigned char GPT_CheckTimeIsElapsed(void){

	// Overflow or Underflow occurred in the first condition
	if(TIM2->SR & 0x1){
		CLEAR_BIT(TIM2->SR, 0);
		return 1;
	}
	// Else there is no overflow so return 0
	else{
		return 0;
	}
}


/*
 * A function to return number of elapsed ticks from the last call of the GPT_StartTimer
 * returns 0 if it is not called and 0xffffffff if an overflow occurred.
 */
unsigned long int GPT_GetElapsedTime(void){

	if(timerStarted == 0){
		return 0;
	}

	else if(TIM2->SR & 0x1){
		return 0xffffffff;
	}

	else{
		return TIM2->CNT;
	}
}

/*
 * A function to return number of remaining ticks till the overflow ticks passed to GPT_StartTimer
 * returns 0xffffffff if GPT_startTime is not called, 0 if an overflow occurred
 */
unsigned long int GPT_GetRemainingTime(void){

	if(timerStarted == 0){
		return 0xffffffff;
	}

	else if(TIM2->SR & 0x1){
		return 0;
	}

	else{
		return TIM2->ARR - TIM2->CNT;
	}
}
