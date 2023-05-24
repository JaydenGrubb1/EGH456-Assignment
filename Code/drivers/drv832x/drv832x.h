#ifndef drv832x_h__
#define drv832x_h__

#include <stdint.h>
#include <stdbool.h>

/// Maximum acceleration under normal conditions
#define DRV832X_MAX_ACCELERATION_RPM 500

/// Maximum deceleration under normal conditions
#define DRV832X_MAX_DECELERATION_RPM 500

/// Target deceleration for an e-stop
#define DRV832X_ESTOP_DECELERATION_RPM 1000

/// Number of bytes to allocate for the stack of the motor drivers tasks
#define DRV832X_TASK_STACK_SIZE 256

/// Number of rising and falling edges seen on the hall effect sensors for 1 rotation of the motor
#define DRV832X_HALLEFFECT_EDGES_PER_ROTATION 26

/// Resolution of the speed sensor in ticks
#define DRV832X_CONTROLLER_PERIOD 5

#define DRV832X_CONTROLLER_GAIN 0.01
#define DRV832X_CONTROLLER_GAIN_P 0.01
#define DRV832X_CONTROLLER_GAIN_I 0.00001

/// Pins used by the motor control driver.
typedef struct _drv832x_Pins
{
    uint32_t hallA;
    uint32_t hallB;
    uint32_t hallC;
} drv832x_Pins;

/// Configuration options passed to drv382x_init
/// used to initialize the motor driver.
typedef struct _drv832x_Config
{
    drv832x_Pins pin;
    uint32_t pwmPeriod;
} drv832x_Config;

typedef enum _drv832x_State
{
    drv832x_Unknown,   ///< The motor state is unknown. Likely the driver has not been initialized yet.
    drv832x_Idle,      ///< The motor is idle (speed == 0)
    drv832x_Starting,  ///< The motor is starting up (i.e. accelerating from 0 RPM to the target RPM)
    drv832x_Running,   ///< The motor is currently running.
    drv832x_EStopping, ///< The motor is performing an emergency stop.
} drv832x_State;

/// Initialize the motor driver config struct with reasonable default values.
// @param pConfig The config struct to configure.
void drv832x_Config_init(drv832x_Config *pConfig);

/// Initialize the motor driver.
/// @pre The boards GPIO API has been initialized
/// @param pConfig Struct with configuration options for the motor control driver.
/// @retval true  The motor driver was initialized successfully.
/// @retval false The motor driver was not initialized successfully.
bool drv832x_init(drv832x_Config const * pConfig);

/// Set the speed of the motor in RPM.
/// @pre drv328x_init has been called successfully.
/// @param rpm The target RPM for the motor
void drv832x_setSpeed(uint32_t rpm);

/// Start the motor.
/// @pre drv328x_init has been called successfully and the driver is in the drv832x_Idle state.
/// @retval true  The motor was successfully started.
/// @retval false The motor could not be started. This is likely because the driver is not in the drv832x_Idle state.
/// @post The driver is in the drv832x_Starting state.
bool drv832x_start();

/// Stop the motor.
/// @pre drv328x_init has been called successfully.
void drv832x_stop();

/// Get the current speed of the motor.
/// @pre drv328x_init has been called successfully.
/// @returns The current speed of the motor in RPM.
uint32_t drv832x_getSpeed();

/// Trigger an emergency stop.
/// @pre drv328x_init has been called successfully.
void drv832x_estop();

/// Query the current state of the motor driver.
/// @pre drv328x_init has been called successfully.
/// @returns The current state of the motor (see drv832x_State).
/// @retval drv832x_Unknown The motor driver has not been initialized.
drv832x_State drv832x_getState();

#endif
