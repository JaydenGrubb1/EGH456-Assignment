#pragma once
#include <stdint.h>

/**
 * @brief GUI callback options
 *
 */
typedef enum GUICallbackOption {
	GUI_MOTOR_STATE_CHANGE = 0,
	GUI_MOTOR_SPEED_CHANGE,
	GUI_MAX_POWER_CHANGE,
	GUI_MAX_ACCEL_CHANGE,
	GUI_SET_TIME_CHANGE,
	GUI_RETURN_RPM,
	GUI_RETURN_POWER,
	GUI_RETURN_LIGHT,
	GUI_RETURN_ACCEL,
	GUI_RETURN_TIME,

	GUI_CALLBACK_COUNT
} tGUICallbackOption;

/**
 * @brief Possible current panels
 *
 */
typedef enum tCurrentPanel {
	MAIN_PANEL,
	SETTINGS_PANEL,
	GRAPH_PANEL
} tCurrentPanel;

/**
 * @brief GUI callback function type
 *
 */
typedef uint32_t (*tGUICallbackFxn)(uint32_t arg1, uint32_t arg2);

/**
 * @brief Initialize the GUI
 *
 * @param ui32SysClock The frequency of the system clock
 */
void GUI_Init(uint32_t ui32SysClock);

/**
 * @brief Triggers a periodic GUI update
 *
 * @note This function should be called periodically from a clock task
 *
 */
void GUI_Pulse();

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
 * @param arg1 The first argument to pass to the callback (optional)
 * @param arg2 The second argument to pass to the callback (optional)
 * @return The result of the callback function (optional)
 *
 * @note This function is not intended to be called by the user
 */
uint32_t GUI_InvokeCallback(tGUICallbackOption tCallbackOpt, uint32_t arg1, uint32_t arg2);

/**
 * @brief Starts the GUI
 *
 */
void GUI_Start();
