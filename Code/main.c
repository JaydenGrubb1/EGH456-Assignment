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

/* GRLib header files */
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"

/* Driver header files */
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"

/* Global defines */
#define TASK_STACK_SIZE 1024

/* Task structs */
Task_Struct g_sWidgetMsgQueueTask;

/* Task stacks */
char ga_cWidgetMsgQueueStack[TASK_STACK_SIZE];

/* Global variables */
tContext g_sContext;
tRectangle g_sScreenRect;

/**
 * @brief Handles processing the messages for the widget message queue
 *
 * @param arg0 Unused
 * @param arg1 Unused
 */
void widgetMsgQueueFxn(UArg arg0, UArg arg1) {
	while (1) {
		WidgetMessageQueueProcess();
	}
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

	/* Initialize touch screen */
	Types_FreqHz cpuFreq;
	BIOS_getCpuFreq(&cpuFreq);
	Kentec320x240x16_SSD2119Init(cpuFreq.lo);
	GrContextInit(&g_sContext, &g_sKentec320x240x16_SSD2119);
	TouchScreenInit(cpuFreq.lo);
	TouchScreenCallbackSet(WidgetPointerMessage);

	/* Initialize screen rect */
	g_sScreenRect.i16XMin = 0;
	g_sScreenRect.i16YMin = 0;
	g_sScreenRect.i16XMax = GrContextDpyWidthGet(&g_sContext);
	g_sScreenRect.i16YMax = GrContextDpyHeightGet(&g_sContext);

	/* Construct task threads */
	Task_Params taskParams;
	Task_Params_init(&taskParams);
	taskParams.stackSize = TASK_STACK_SIZE;
	taskParams.stack = &ga_cWidgetMsgQueueStack;
	Task_construct(&g_sWidgetMsgQueueTask, (Task_FuncPtr)widgetMsgQueueFxn, &taskParams, NULL);

	/* Start BIOS */
	BIOS_start();
	return (0);
}
