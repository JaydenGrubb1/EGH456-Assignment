#ifndef TimeSampler_h__
#define TimeSampler_h__

#include <stdint.h>
#include <stdbool.h>

#include "HallSensor.h"

#define SPEED_MAX_SAMPLES 5

typedef enum _SensorState {
    SensorState_Unknown = -1,
    SensorState_Low,
    SensorState_High,
    SensorState_Rising,
    SensorState_Falling,
    SensorState_Count,
} SensorState;

typedef struct _TimeSampler {
    /// How long until a sample is discarded
    uint32_t sampleExpireTime;
    /// How long until a sample is discarded
    uint32_t totalSamples;
    /// Offset of first sample in the sample buffer
    uint32_t startOffset;
    /// Current number of samples
    uint32_t sampleCount;
    /// When a sample is no longer valid
    uint32_t buffer[SPEED_MAX_SAMPLES];
} TimeSampler;

void TimeSampler_init(TimeSampler *pSensing);

/// Get the true index of a sample in the buffer.
/// The sample buffer is a ring buffer, so the start moves as samples are popped.
uint32_t TimeSampler_getSampleIndex(TimeSampler const * pSensing, uint32_t sample);

/// Remove a sample from the start of the sample buffer
bool TimeSampler_popSample(TimeSampler *pSensing);

/// Add a sample to the end of the sample buffer.
void TimeSampler_addSample(TimeSampler *pSensing, uint32_t time);

/// Remove samples from the buffer up until `time`
void TimeSampler_discardExpiredSamples(TimeSampler *pSensing, uint32_t time);

/// Calculate motor speed using data in `pSensing`
float TimeSampler_calculateSpeed(TimeSampler const * pSensing);

/// Current number of samples in the buffer
uint32_t TimeSampler_numSamples(TimeSampler const * pSensing);

/// Total number of samples that have been added.
uint32_t TimeSampler_totalSamples(TimeSampler const * pSensing);

#endif // TimeSampler_h__
