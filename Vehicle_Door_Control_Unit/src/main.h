#ifndef MAIN_H_
#define MAIN_H_

#include "Std_Types.h"

#define TicksPerSecond 	1000

//Used Pins in Gpio
#define BTN_HANDLE		0
#define	BTN_DOOR		1
#define LED_VEHICLE     2
#define LED_HAZARD    	3
#define LED_AMBIENT     4

// App states
uint8 unlocked;
uint8 opened;

// Buttons states
uint8 handle_btn_currentState;
uint8 handle_btn_previousState;
uint8 door_btn_currentState;
uint8 door_btn_previousState;

// LEDs state struct
typedef struct {
	uint8 ambient_always_on;
	uint8 ambient_time;
	uint8 num_hazard_blinks;
	uint8 vehicle;
	uint8 ambient_on_flag;
	uint8 hazard_on_flag;
	uint8 vehicle_on_flag;


} LEDs_STATE;


// Functions
void driveLEDs(void);
void unlockHandle(void);
void lockHandle(void);
void closeDoor(void);
void openDoor(void);
uint8 checkHandleBTN(void);
uint8 checkDoorBTN(void);
#endif /* MAIN_H_ */
