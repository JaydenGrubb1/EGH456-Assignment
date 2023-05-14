#pragma once
#include <stdint.h>

/**
 * @brief GUI callback options
 *
 */
typedef enum GUICallbackOption {
	GUI_MOTOR_START = 0,
	GUI_MOTOR_STOP,
	GUI_MOTOR_STATE_CHANGE,
	GUI_MOTOR_SPEED_CHANGE,

	GUI_CALLBACK_COUNT
} tGUICallbackOption;

/**
 * @brief GUI callback function type
 *
 */
typedef void (*tGUICallbackFxn)(uint32_t arg1, uint32_t arg2);

/**
 * @brief Initialize the GUI
 *
 * @param ui32SysClock The frequency of the system clock
 */
void GUI_Init(uint32_t ui32SysClock);

/**
 * @brief Handles processing the messages for the widget message queue
 *
 * @note This function does not return and should be called in its own task
 *
 */
void GUI_Handle();

/**
 * @brief Sets the callback function for a specific callback
 *
 * @param tCallbackOpt The callback to set
 * @param pfnCallbackFxn The function to call when the callback is triggered
 */
void GUI_SetCallback(tGUICallbackOption tCallbackOpt, tGUICallbackFxn pfnCallbackFxn);

/**
 * @brief Invokes the callback function for a specific callback
 * 
 * @param tCallbackOpt The callback to invoke
 * @param arg1 The first argument to pass to the callback
 * @param arg2 The second argument to pass to the callback
 * 
 * @note This function is not intended to be called by the user
 */
void GUI_InvokeCallback(tGUICallbackOption tCallbackOpt, uint32_t arg1, uint32_t arg2);

/**
 * @brief Starts the GUI
 *
 */
void GUI_Start();
