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
#define GUI_PULSE_PERIOD 500

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

/**
 * @brief Callback function for when the time settings change
 *
 * @param hours The new hours value
 * @param minutes The new minutes value
 */
void TimeChanged(uint32_t hours, uint32_t minutes) {
	// System_printf("Time changed to %02d:%02d\n", hours, minutes);
	// System_flush();
}

/**
 * @brief Callback function to get the current RPM
 *
 * @return The current RPM
 */
int16_t GetCurrentRPM() {
	return rand() / 100;
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
	GUI_SetCallback(GUI_SET_TIME_CHANGE, (tGUICallbackFxn)TimeChanged);
	GUI_SetCallback(GUI_RETURN_RPM, (tGUICallbackFxn)GetCurrentRPM);

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
