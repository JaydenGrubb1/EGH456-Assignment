/* Standard header files */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h> // rand()
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

/* Global defines */
#define TASK_STACK_SIZE 1024
#define GUI_PULSE_PERIOD 100

/* Task structs */
Task_Struct g_sHandleGUITask;

/* Task stacks */
char ga_cHandleGUIStack[TASK_STACK_SIZE];

/**
 * @brief Callback function for when the motor state changes
 *
 * @param bMotorState The new state of the motor
 */
void MotorStateChanged(bool bMotorState) {
	GPIO_write(Board_LED0, bMotorState);
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
	return ((sin(lightCounter / 20) * 40) + (255 / 2));
}

float accelCounter = 0;
int16_t GetCurrentAccel() {
	accelCounter++;
	return (((sin(accelCounter / 20) * 40) + (255 / 2)) + ((sin(accelCounter / 10) * 80) + (255 / 2))) / 2;
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
	GUI_SetCallback(GUI_RETURN_SPEED, (tGUICallbackFxn)GetCurrentSpeed);
	GUI_SetCallback(GUI_RETURN_POWER, (tGUICallbackFxn)GetCurrentPower);
	GUI_SetCallback(GUI_RETURN_LIGHT, (tGUICallbackFxn)GetCurrentLight);
	GUI_SetCallback(GUI_RETURN_ACCEL, (tGUICallbackFxn)GetCurrentAccel);

	/* Construct task threads */
	Task_Params taskParams;
	Task_Params_init(&taskParams);
	taskParams.stackSize = TASK_STACK_SIZE;
	taskParams.stack = &ga_cHandleGUIStack;
	Task_construct(&g_sHandleGUITask, GUI_Handle, &taskParams, NULL);

	/* Construct clock threads */
	Clock_Params clockParams;
	Clock_Params_init(&clockParams);
	clockParams.period = GUI_PULSE_PERIOD;
	clockParams.startFlag = true;
	Clock_create(GUI_Pulse, GUI_PULSE_PERIOD, &clockParams, NULL);

	/* Start the GUI */
	GUI_Start();

	/* Start BIOS */
	BIOS_start();
	return (0);
}
