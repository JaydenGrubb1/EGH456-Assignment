/* Standard header files */
#include <stdio.h>
#include <stdlib.h> // rand()
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

/* XDCtools header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>

/* BIOS header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

/* TI-RTOS header files */
#include <ti/drivers/GPIO.h>

/* Board header file */
#include "Board.h"

/* Project header files */
#include "gui.h"
#include "util.h"
#include "config.h"

/* Global defines */
#define TASK_STACK_SIZE 1024

/* Global variables */
Task_Struct g_sHandleGUITask;
char ga_cHandleGUIStack[TASK_STACK_SIZE];
uint32_t g_ui32ClockCounter = 0;

/**
 * @brief Callback function for when the motor state changes
 *
 * @param bMotorState The new state of the motor
 */
void MotorStateChanged(bool bMotorState) {
	// TODO: Implement this
}

/**
 * @brief Pulse the clock and increment the counter
 *
 */
void PulseClock() {
	// TODO: Add semaphore
	g_ui32ClockCounter++;
	if (g_ui32ClockCounter > 86400)
		g_ui32ClockCounter = 0;
}

/**
 * @brief Get the current clock counter value
 *
 * @return The current clock counter value
 */
uint32_t GetClock() {
	// TODO: Add semaphore
	return g_ui32ClockCounter;
}

/**
 * @brief Set the clock counter value
 *
 * @param ui32Time The new clock counter value
 */
void SetClock(uint32_t ui32Time) {
	// TODO: Add semaphore
	g_ui32ClockCounter = ui32Time;
}

float rpmCounter = 0;
int16_t GetCurrentSpeed() {
	rpmCounter++;
	return ((sin(rpmCounter / 5) * 80) + (255 / 2));
}

float powerCounter = 0;
int16_t GetCurrentPower() {
	powerCounter++;
	return ((sin(powerCounter / 10) * 80) + (255 / 2));
}

float lightCounter = 0;
int16_t GetCurrentLight() {
	lightCounter++;
	return ((sin(lightCounter / (38.5 / M_PI)) * 40) + 35);
}

float accelCounter = 0;
int16_t GetCurrentAccel() {
	accelCounter++;
	return (((sin(accelCounter / 2) * 40) + (255 / 2)) + ((sin(accelCounter / 10) * 80) + (255 / 2))) / 2;
}

bool GetEStop() {
	// uint32_t ticks = Clock_getTicks();		// Enables e-stop after 10 seconds
	// return ticks < 20000 && ticks > 10000;	// Disables e-stop after 20 seconds
	return false;
}

/**
 * @brief Application entry point
 *
 * @return Unused
 */
int main(void) {
	/* Call board init functions */
	Board_initGeneral();
	Board_initGPIO();

	/* Get CPU frequency */
	Types_FreqHz cpuFreq;
	BIOS_getCpuFreq(&cpuFreq);

	/* Initialize the GUI */
	GUI_Init(cpuFreq.lo);
	GUI_SetCallback(GUI_MOTOR_STATE_CHANGE, (tGUICallbackFxn)MotorStateChanged);
	GUI_SetCallback(GUI_SET_TIME_CHANGE, (tGUICallbackFxn)SetClock);
	GUI_SetCallback(GUI_RETURN_TIME, (tGUICallbackFxn)GetClock);
	GUI_SetCallback(GUI_RETURN_SPEED, (tGUICallbackFxn)GetCurrentSpeed);
	GUI_SetCallback(GUI_RETURN_POWER, (tGUICallbackFxn)GetCurrentPower);
	GUI_SetCallback(GUI_RETURN_LIGHT, (tGUICallbackFxn)GetCurrentLight);
	GUI_SetCallback(GUI_RETURN_ACCEL, (tGUICallbackFxn)GetCurrentAccel);
	GUI_SetCallback(GUI_RETURN_ESTOP, (tGUICallbackFxn)GetEStop);

	/* Construct task threads */
	Task_Params taskParams;
	Task_Params_init(&taskParams);
	taskParams.stackSize = TASK_STACK_SIZE;
	taskParams.stack = &ga_cHandleGUIStack;
	Task_construct(&g_sHandleGUITask, (Task_FuncPtr)GUI_Handle, &taskParams, NULL);

	/* Construct clock threads */
	Clock_Params clockParams;
	Clock_Params_init(&clockParams);
	clockParams.startFlag = true;
	clockParams.period = GUI_PULSE_PERIOD;
	Clock_create((Clock_FuncPtr)GUI_Pulse, GUI_PULSE_PERIOD, &clockParams, NULL);
	clockParams.period = 1000;
	Clock_create((Clock_FuncPtr)PulseClock, 1000, &clockParams, NULL);

	/* Start the GUI */
	GUI_Start();

	/* Start BIOS */
	BIOS_start();
	return (0);
}
