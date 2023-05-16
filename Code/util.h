#pragma once
#include <stdint.h>

/**
 * @brief Convert a time to ticks
 * 
 * @param hour The hour portion of the time
 * @param minute The minute portion of the time
 * @param second The second portion of the time
 * @return The number of ticks
 */
uint32_t TimeToTicks(uint8_t hour, uint8_t minute, uint8_t second);

/**
 * @brief Convert ticks to a time
 * 
 * @param ticks The number of ticks
 * @param hour The hour portion of the time
 * @param minute The minute portion of the time
 * @param second The second portion of the time
 */
void TicksToTime(uint32_t ticks, uint8_t *hour, uint8_t *minute, uint8_t *second);
