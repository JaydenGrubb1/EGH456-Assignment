/* Standard header files */
#include <stdio.h>
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

/* TI-RTOS header files */
#include <ti/drivers/GPIO.h>

/* Board header file */
#include "Board.h"

/* Project header files */
#include "gui.h"

/* Global defines */
#define TASK_STACK_SIZE 1024

/* Task structs */
Task_Struct g_sHandleGUITask;

/* Task stacks */
char ga_cHandleGUIStack[TASK_STACK_SIZE];

/**
 * @brief Example callback function for when the motor is started
 * 
 */
void MotorStarted() {
	System_printf("Motor started\n");
	System_flush();
}

/**
 * @brief Example callback function for when the motor is stopped
 * 
 */
void MotorStopped() {
	System_printf("Motor stopped\n");
	System_flush();
}

/**
 * @brief Example callback function for when the motor state is changed
 * 
 * @param bMotorState The new state of the motor
 */
void MotorStateChanged(bool bMotorState) {
	System_printf("Motor state changed: ");
	if (bMotorState) {
		System_printf("Motor is on\n");
	} else {
		System_printf("Motor is off\n");
	}
	System_flush();
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
	GUI_SetCallback(GUI_MOTOR_START, (tGUICallbackFxn)MotorStarted);
	GUI_SetCallback(GUI_MOTOR_STOP, (tGUICallbackFxn)MotorStopped);
	GUI_SetCallback(GUI_MOTOR_STATE_CHANGE, (tGUICallbackFxn)MotorStateChanged);

	/* Construct task threads */
	Task_Params taskParams;
	Task_Params_init(&taskParams);
	taskParams.stackSize = TASK_STACK_SIZE;
	taskParams.stack = &ga_cHandleGUIStack;
	Task_construct(&g_sHandleGUITask, (Task_FuncPtr)GUI_Handle, &taskParams, NULL);

	/* Draw the GUI */
	GUI_Start();

	/* Start BIOS */
	BIOS_start();
	return (0);
}
