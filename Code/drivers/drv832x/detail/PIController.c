#include "PIController.h"

void PIController_init(PIController *pController, float gainP, float gainI)
{
    pController->gainP = gainP;
    pController->gainI = gainI;
    pController->lastUpdateTick = 0;
    pController->sampleCount = 0;
}


void PIController_reset(PIController *pController)
{
    pController->sampleCount = 0;
    pController->lastUpdateTick = 0;
    pController->integral = 0;
    pController->signal = 0;
}

void PIController_setTarget(PIController * pController, int32_t rpm)
{
    pController->targetRpm = rpm;
}

int32_t PIController_getTarget(PIController const * pController)
{
    return pController->targetRpm;
}

void PIController_update(PIController *pController, int32_t currentRpm, uint32_t currentTick)
{
    int32_t err = pController->targetRpm - currentRpm;
    pController->integral += ((int32_t)(currentTick - pController->lastUpdateTick)) * (pController->lastError + err) / 2;
    float p = pController->gainP * err;
    float i = pController->gainI * pController->integral;
    pController->signal = p + i;

    pController->lastError = err;
    pController->lastUpdateTick = currentTick;
}

float PIController_getSignal(PIController const * pController)
{
    return pController->signal;
}
