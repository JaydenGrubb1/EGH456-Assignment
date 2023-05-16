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

/**
 * @brief Linear interpolation between two values
 *
 * @param a The first value
 * @param b The second value
 * @param t The interpolation factor
 * @return The interpolated value
 */
float Lerp(float a, float b, float t);

/**
 * @brief Inverse linear interpolation between two values
 *
 * @param a The first value
 * @param b The second value
 * @param v The value to interpolate
 * @return The interpolation factor
 */
float InverseLerp(float a, float b, float v);

/**
 * @brief Map a value from one range to another
 *
 * @param x The value to map
 * @param in_min The minimum value of the input range
 * @param in_max The maximum value of the input range
 * @param out_min The minimum value of the output range
 * @param out_max The maximum value of the output range
 * @return The mapped value
 */
float Map(float x, float in_min, float in_max, float out_min, float out_max);
