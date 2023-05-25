#ifndef PIController_h__
#define PIController_h__

#include <stdint.h>
#include <stdbool.h>

typedef struct _PIController {
    float gainP; /// Gain for the proportional signal component.
    float gainI; /// Gain for the integral component.

    float integral;
    float signal;

    int32_t lastError;
    uint32_t lastUpdateTick;
    bool empty;
} PIController;

/// Initialise the controller with the specified gains.
/// @param gainP The gain to apply to the proportional error.
/// @param gainI The gain to apply to the integral of the error.
void PIController_init(PIController *pController, float gainP, float gainI);

/// Reset the controller.
void PIController_reset(PIController *pController, float gainP, float gainI);

/// Update the speed controller.
/// @param currentRpm  The current speed of the motor.
/// @param currentTick The time that the update occured.
void PIController_update(PIController *pController, int32_t targetRpm, int32_t currentRpm, uint32_t currentTick);

/// Get the input signal for the motor.
/// Returns a value between 0 and 1.
/// The caller is responsible for scaling this to the appropriate pwm duty cycle.
float PIController_getSignal(PIController const * pController);

#endif // PIController_h__
