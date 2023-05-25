#include "PIController.h"

void PIController_init(PIController *pController, float gainP, float gainI)
{
    pController->gainP = gainP;
    pController->gainI = gainI;
    pController->lastUpdateTick = 0;
    pController->lastError = 0;
    pController->empty = true;
}

void PIController_reset(PIController *pController, float gainP, float gainI)
{
    pController->gainP = gainP;
    pController->gainI = gainI;
    pController->lastUpdateTick = 0;
    pController->empty = true;
    pController->integral = 0;
    pController->lastError = 0;
    pController->signal = 0;
}

void PIController_update(PIController *pController, int32_t targetRpm, int32_t currentRpm, uint32_t currentTick)
{
    int32_t err = targetRpm - currentRpm;
    if (!pController->empty)
        pController->integral += ((int32_t)(currentTick - pController->lastUpdateTick)) * (pController->lastError + err) / 2;
    float p = pController->gainP * err;
    float i = pController->gainI * pController->integral;
    pController->signal = p + i;

    pController->lastError = err;
    pController->lastUpdateTick = currentTick;

    pController->empty = false;
}

float PIController_getSignal(PIController const * pController)
{
    return pController->signal;
}
