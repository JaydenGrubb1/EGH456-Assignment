#pragma once
#include <stdint.h>

/**
 * @brief Convert the time given in hours, minutes and seconds to ticks (total seconds)
 * 
 * @param hour The hour portion of the time
 * @param minute The minute portion of the time
 * @param second The second portion of the time
 * @return The number of ticks (total seconds)
 */
uint32_t TimeToTicks(uint8_t hour, uint8_t minute, uint8_t second);

/**
 * @brief Convert ticks (total seconds) to time given in hours, minutes and seconds
 * 
 * @param ticks The number of ticks (total seconds)
 * @param hour The hour portion of the time
 * @param minute The minute portion of the time
 * @param second The second portion of the time
 */
void TicksToTime(uint32_t ticks, uint8_t *hour, uint8_t *minute, uint8_t *second);
