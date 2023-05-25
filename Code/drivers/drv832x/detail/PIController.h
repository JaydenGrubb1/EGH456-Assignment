#ifndef PIController_h__
#define PIController_h__

#include <stdint.h>

typedef struct _PIController {
    float gainP; /// Gain for the proportional signal component.
    float gainI; /// Gain for the integral component.

    float integral;
    float signal;

    int32_t lastError;
    int32_t targetRpm;
    uint32_t lastUpdateTick;
    uint32_t sampleCount;    /// Total number of updates
} PIController;

/// Initialise the controller with the specified gains.
/// @param gainP The gain to apply to the proportional error.
/// @param gainI The gain to apply to the integral of the error.
void PIController_init(PIController *pController, float gainP, float gainI);

/// Reset the controller.
void PIController_reset(PIController *pController);

/// Set the target RPM.
/// @param rpm The target RPM.
void PIController_setTarget(PIController *pController, int32_t rpm);

/// Get the target RPM.
/// @return The target RPM.
int32_t PIController_getTarget(PIController const * pController);

/// Update the speed controller.
/// @param currentRpm  The current speed of the motor.
/// @param currentTick The time that the update occured.
void PIController_update(PIController *pController, int32_t currentRpm, uint32_t currentTick);

/// Get the input signal for the motor.
/// Returns a value between 0 and 1.
/// The caller is responsible for scaling this to the appropriate pwm duty cycle.
float PIController_getSignal(PIController const * pController);

#endif // PIController_h__
