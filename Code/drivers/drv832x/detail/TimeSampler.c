#include "TimeSampler.h"

#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>

void TimeSampler_init(TimeSampler *pSampler)
{
    pSampler->sampleCount = 0;
    pSampler->sampleExpireTime = 100;
    pSampler->startOffset = 0;
    memset(pSampler->buffer, sizeof(pSampler->buffer), 0);
}

uint32_t TimeSampler_getSampleIndex(TimeSampler const * pSampler, uint32_t sample)
{
    return (pSampler->startOffset + sample) % SPEED_MAX_SAMPLES;
}

bool TimeSampler_popSample(TimeSampler *pSampler)
{
    if (pSampler->sampleCount == 0)
        return false;
    pSampler->buffer[pSampler->startOffset] = 0;
    pSampler->startOffset = (pSampler->startOffset + 1) % SPEED_MAX_SAMPLES;
    pSampler->sampleCount--;
    return true;
}

void TimeSampler_addSample(TimeSampler *pSampler, uint32_t time)
{
    // Remove an old sample if 
    if (pSampler->sampleCount == SPEED_MAX_SAMPLES)
        TimeSampler_popSample(pSampler);

    uint32_t index = TimeSampler_getSampleIndex(pSampler, pSampler->sampleCount);
    pSampler->buffer[index] = time;
    pSampler->sampleCount++;
}

void TimeSampler_discardExpiredSamples(TimeSampler *pSampler, uint32_t time)
{
    while (pSampler->sampleCount > 0 && time > pSampler->buffer[pSampler->startOffset] + pSampler->sampleExpireTime)
        TimeSampler_popSample(pSampler);
}

float TimeSampler_calculateSpeed(TimeSampler const * pSampler)
{
    if (pSampler->sampleCount < 2)
        return 0;

    uint32_t first = TimeSampler_getSampleIndex(pSampler, 0);
    uint32_t last  = TimeSampler_getSampleIndex(pSampler, pSampler->sampleCount - 1);
    uint32_t start = pSampler->buffer[first];
    uint32_t end   = pSampler->buffer[last];

    float avgEdgeTicks = (float)(end - start) / pSampler->sampleCount;
    if (avgEdgeTicks == 0)
        return 0; // Too fast to detect or not enough samples yet

    return avgEdgeTicks;
}

uint32_t TimeSampler_numSamples(TimeSampler const * pSampler)
{
    return pSampler->sampleCount;
}

uint32_t TimeSampler_totalSamples(TimeSampler const * pSampler)
{
    return pSampler->totalSamples;
}
