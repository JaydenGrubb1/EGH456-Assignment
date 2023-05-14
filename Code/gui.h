#pragma once
#include <stdint.h>

/**
 * @brief Initialize the GUI
 * 
 * @param ui32SysClock The frequency of the system clock
 */
void InitGUI(uint32_t ui32SysClock);

/**
 * @brief Handles processing the messages for the widget message queue
 *
 * @note This function does not return and should be called in its own task
 * 
 */
void HandleGUI();

/**
 * @brief Draws the GUI
 * 
 */
void DrawGUI();
