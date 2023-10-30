#include "main.h"

// Drivers
#include "Rcc.h"
#include "Gpio.h"
#include "GPT.h"

// Libraries
#include "Bit_Operations.h"


// States (global variables)
LEDs_STATE LED = {0,0,0,0,0,0,0};

uint8 current_state [2][2] = {{0, 5}, {1, 2}};
/*
 *  [0][0] => handle locked and door closed
 * 	[0][1] => out of scope (not possible)
 * 	[1][0] => handle unlocked and door closed
 * 	[1][1] => handle unlocked and door opened
 */


int main() {

//	Enable the GPIO used pin
	Rcc_Init();
	Rcc_Enable(RCC_GPIOA);

//	Initialize Gpio_A pins
	uint8 i;

//	Configure Input buttons
	for (i = 0; i <= 1; i++) {
		Gpio_ConfigPin(GPIO_A, i, GPIO_INPUT, GPIO_PULL_UP);
	}

//	Configure Output LEDs
	for (i = 2; i <= 4; i++) {
		Gpio_ConfigPin(GPIO_A, i, GPIO_OUTPUT, GPIO_PUSH_PULL);
	}


//	Initialize the GPT
	GPT_Init();

//	Default State of the application (All LEDs Off)
	unlocked = 0;
	opened = 0;
	uint8 state;

	handle_btn_currentState = 1;
	handle_btn_previousState = 1;
	door_btn_currentState = 1;
	door_btn_previousState = 1;


	// Main functionality loop
	while (1) {

		driveLEDs();

/*		To get the current state at which is the application
*		and to which other state it should go based on the pressed buttons  */
		state = current_state[unlocked][opened];

		switch(state){
			// Case of Door is closed -- Handle is locked
			case 0:
				if (checkHandleBTN()){
					unlockHandle(); //	Unlock the handle with handle button
				}
				break;

			// Case of Door is closed -- Handle is unlocked
			case 1:
				if (GPT_CheckTimeIsElapsed() || checkHandleBTN()){
					lockHandle(); // Lock the handle with Anti-theft or handle button
				}
				else if (checkDoorBTN()){
					openDoor(); //	Open the door with door button
				}
				break;

			// Case of Door is opened
			case 2:
				if (checkDoorBTN()){
					closeDoor(); //	Close the door with door button
				}
				break;

			default:
				break;
		}
	}

	return 0;
}


/*
 * Function that handle all LEDs states and timing of each of them
 */
void driveLEDs(){

	unsigned long int elapsed_time = GPT_GetElapsedTime();

//	Ambient LED
	if ( !(LED.ambient_on_flag) && LED.ambient_always_on){
		Gpio_WritePin(GPIO_A, LED_AMBIENT, HIGH);
		LED.ambient_on_flag = 1;
	}
	else if (LED.ambient_time){


		if( !(LED.ambient_on_flag) && (elapsed_time <= LED.ambient_time * TicksPerSecond) ){
			Gpio_WritePin(GPIO_A, LED_AMBIENT, HIGH);
			LED.ambient_on_flag = 1;
		}

		else if (LED.ambient_on_flag && (elapsed_time > LED.ambient_time * TicksPerSecond)){
			Gpio_WritePin(GPIO_A, LED_AMBIENT, LOW);
			LED.ambient_on_flag = 0;
		}
	}

	else if (LED.ambient_on_flag && !(LED.ambient_always_on) ) {
		Gpio_WritePin(GPIO_A, LED_AMBIENT, LOW);
		LED.ambient_on_flag = 0;
	}

//	Hazard LED
	if (LED.num_hazard_blinks){

		uint8 blinkHigh = ( elapsed_time <= 500 || (elapsed_time > 1000 && elapsed_time <= (500 + 1000*(LED.num_hazard_blinks - 1))));

		if ( !(LED.hazard_on_flag) && blinkHigh){
			Gpio_WritePin(GPIO_A, LED_HAZARD, HIGH);
			LED.hazard_on_flag = 1;
		}
		else if (LED.hazard_on_flag && !blinkHigh) {
			Gpio_WritePin(GPIO_A, LED_HAZARD, LOW);
			LED.hazard_on_flag = 0;
		}
	}
	else if ( LED.hazard_on_flag && !(LED.num_hazard_blinks) ){
		Gpio_WritePin(GPIO_A, LED_HAZARD, LOW);
		LED.hazard_on_flag = 0;
	}


//	Vehicle LED
	if ( !(LED.vehicle_on_flag) && LED.vehicle ){
		Gpio_WritePin(GPIO_A, LED_VEHICLE, HIGH);
		LED.vehicle_on_flag = 1;
	}
	else if ( LED.vehicle_on_flag && !(LED.vehicle)){
		Gpio_WritePin(GPIO_A, LED_VEHICLE, LOW);
		LED.vehicle_on_flag = 0;
	}

}


/*
 * Function that unlocks the door handle by enabling Vehicle LED & blinking Hazard LED one time 0.5 sec high and 0.5 low
 * while Ambient light LED is on for 2 seconds then off.
 */
void unlockHandle(){
	LED.ambient_always_on = 0;
	LED.ambient_time = 2;
	LED.num_hazard_blinks = 1;
	LED.vehicle = 1;

	GPT_StartTimer(10 * TicksPerSecond);

	unlocked =	1;
}

/*
 * Function that locks the door handle by disabling Vehicle LED & blinking Hazard LED two times 0.5 sec high and 0.5 low
 * while Ambient light LED is off.
 */
void lockHandle(){
	LED.ambient_always_on = 0;
	LED.ambient_time = 0;
	LED.num_hazard_blinks = 2;
	LED.vehicle = 0;

	GPT_StartTimer(2 * TicksPerSecond);

	unlocked =	0;
}

/*
 * Function that opens the door by enabling the ambient light
 */
void openDoor(){
	LED.ambient_always_on = 1;
	LED.ambient_time = 0;
	LED.num_hazard_blinks = 0;
	LED.vehicle = 0;

	opened = 1;
}


/*
 * Function that closes the door by disabling Vehicle LED & Hazard LED
 * while Ambient light LED is on for one second and then is off.
 */
void closeDoor(){
	LED.ambient_always_on = 0;
	LED.ambient_time = 1;
	LED.num_hazard_blinks = 0;
	LED.vehicle = 0;

	GPT_StartTimer(10 * TicksPerSecond);

	opened = 0;
}


/*
 * Function to read buttons at Falling Edges
 */
uint8 checkHandleBTN(){

	uint8 check = 0;

	handle_btn_currentState = Gpio_ReadPin(GPIO_A, BTN_HANDLE);

	GPT_StartTimer(100); //	wait 100ms for debounce
	while(!( GPT_CheckTimeIsElapsed() ) );

	if ( handle_btn_currentState == Gpio_ReadPin(GPIO_A, BTN_HANDLE) ){

		if (handle_btn_currentState != handle_btn_previousState) {

	            if (handle_btn_currentState == LOW) {
	                check = 1;
	            }

	            // Update the previous state
	            handle_btn_previousState = handle_btn_currentState;
	        }
	}

	return check;
}

uint8 checkDoorBTN(){

	uint8 check=0;

	door_btn_currentState = Gpio_ReadPin(GPIO_A, BTN_DOOR);

	GPT_StartTimer(100); //	wait 100ms for debounce
	while(!( GPT_CheckTimeIsElapsed() ) );

	if( door_btn_currentState == Gpio_ReadPin(GPIO_A, BTN_DOOR ){

		if (door_btn_currentState != door_btn_previousState) {
	            if (door_btn_currentState == LOW) {
	                check = 1;
	            }

	            // Update the previous state
	            door_btn_previousState = door_btn_currentState;
	        }
	}

	return check;
}
