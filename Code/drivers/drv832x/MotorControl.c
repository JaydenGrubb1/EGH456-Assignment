#include "MotorControl.h"
#include "motorlib/motorlib.h"
#include "detail/TimeSampler.h"
#include "detail/PIController.h"
#include "Board.h"

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/GPIO.h>

#include <string.h>
#include <float.h>

static MotorControl_Config g_config;

// Current duty cycle for the motor.
// Float so the controller can make slow/small adjustments.
// This is truncated before being applied to the motor (maybe it would be better to round?).
volatile static float g_currentDutyCycle = 0.0f;
volatile static float g_maxDutyIncrease = 0;
volatile static float g_maxDutyDecrease = 0;

volatile static float g_avgRpm = 0.0f;

// Current speed of the motor.
volatile static uint32_t g_currentRpm = 0;

// Target speed of the motor. This is used when starting/running
volatile static uint32_t g_targetRpm = 0;

volatile static uint32_t g_numEdges = 0;
// Current state of the motor driver.
volatile static MotorControl_State g_motorState = MotorControl_State_Unknown;

// Last motor phase detected.
volatile static uint32_t g_currentMotorPhase = 0;

// LUT used to get the phase from the sensor states
volatile static uint8_t g_phaseLUT[8] = {};

/// Responsible for calculating time between hall sensor signal edges.
static TimeSampler g_hallEdgeTimer;

/// PI Controller used to drive the duty cycle motor input
static PIController g_controller;

/// -------------------------
/// State Machine
typedef struct _DriverState
{
    /// Test if the state transition is valid
    bool(*canTransition)(MotorControl_State newState);

    /// Called when this state is activate.
    void(*activate)();

    /// Called when this state is deactivate1
    void(*deactivate)();

    /// Called when during the motor control clock.
    void(*onUpdateMotor)(uint32_t speed, uint32_t tick);
} IStateController;

// Array of state controllers. One for each of the motor states
static IStateController g_states[MotorControl_State_NumStates];

// Get the activate state controller.
static IStateController const * getStateContoller()
{
    return g_states + g_motorState;
}

// Configures state controller interfaces. Implemented below.
static void initStateInterfaces();

// Attempt to transition the motor control state machine to a new state.
bool MotorControl_transition(MotorControl_State state)
{
    // Get the current controller and check if we can transition to `state`
    IStateController const * pController = getStateContoller();
    if (pController != 0)
    {
        if (pController->canTransition != 0 && !pController->canTransition(state))
            return false;

        // Deactivate the current state controller
        if (pController->deactivate != 0)
            pController->deactivate();
    }

    // Transition to the new state
    g_motorState = state;

    // Get the new controller
    pController = getStateContoller();
    if (pController != 0)
    {
        // Activate the new state controller
        if (pController->activate != 0)
            pController->activate();
    }

    return true;
}
/// -------------------------

//----------------------
// Motor controller task
static uint8_t calcPhaseTableIndex(bool a, bool b, bool c)
{
    return ((a & 0b1) << 0)
            | ((b & 0b1) << 1)
            | ((c & 0b1) << 2);
}

static void initMotorPhaseTable()
{
    g_phaseLUT[calcPhaseTableIndex(1, 0, 1)] = 0;
    g_phaseLUT[calcPhaseTableIndex(1, 0, 0)] = 1;
    g_phaseLUT[calcPhaseTableIndex(1, 1, 0)] = 2;
    g_phaseLUT[calcPhaseTableIndex(0, 1, 0)] = 3;
    g_phaseLUT[calcPhaseTableIndex(0, 1, 1)] = 4;
    g_phaseLUT[calcPhaseTableIndex(0, 0, 1)] = 5;
}

static uint8_t getMotorPhase(bool a, bool b, bool c)
{
    return g_phaseLUT[calcPhaseTableIndex(a, b, c)];
}

static void setMotorPhase(uint32_t phase)
{
    updateMotor(
            (phase) < 3,
            ((phase - 2) % 6) < 3,
            ((phase - 4) % 6) < 3);
}

static uint32_t pollMotorPhase()
{
    bool stateA = GPIO_read(g_config.pin.hallA);
    bool stateB = GPIO_read(g_config.pin.hallB);
    bool stateC = GPIO_read(g_config.pin.hallC);

    return getMotorPhase(stateA, stateB, stateC);
}

static Clock_Struct g_controlClock;
static void motorControlClock(UArg arg0)
{
    // We always need to update the speed sensor regardless of state.
    const uint32_t currentTick = Clock_getTicks();
    TimeSampler_discardExpiredSamples(&g_hallEdgeTimer, currentTick);

    // Call current states onUpdateMotor implementation
    IStateController const * pController = getStateContoller();
    if (pController != 0 && pController->onUpdateMotor != 0)
    {
        const uint32_t speed = MotorControl_getSpeed();

        pController->onUpdateMotor(speed, currentTick);
    }

    int numEdges = g_numEdges;
    g_numEdges = 0;

    float edgesPerSec = numEdges * 1000;
    float rotationsPerSec = edgesPerSec / DRV832X_HALLEFFECT_EDGES_PER_ROTATION;
    float rpm = rotationsPerSec * 60;

    static int sampleCount = 25;

    float newAvg = g_avgRpm * (sampleCount - 1);
    g_avgRpm = (newAvg + rpm) / sampleCount;
}
//----------------------

//----------------------
// Motor speed sensing
static void edgeInterruptHandler(unsigned int pin)
{
    TimeSampler_addSample(&g_hallEdgeTimer, Clock_getTicks());
    g_currentMotorPhase = pollMotorPhase();
    setMotorPhase(g_currentMotorPhase + 1);
    ++g_numEdges;
}
//----------------------

void MotorControl_Config_init(MotorControl_Config *pConfig)
{
    pConfig->pin.hallA = Board_MOTOR_HALL_A;
    pConfig->pin.hallB = Board_MOTOR_HALL_B;
    pConfig->pin.hallC = Board_MOTOR_HALL_C;
    pConfig->pwmPeriod = 50;
}

bool MotorControl_init(MotorControl_Config const * pConfig)
{
    Error_Block err;
    if (!initMotorLib(pConfig->pwmPeriod, &err))
    {
        System_printf(err.msg);
        System_flush();
        // TODO: Output error
        return false;
    }

    g_config = *pConfig;

    // Create hi-frequency clock thread for motor control
    Clock_Params clockParams;
    Clock_Params_init(&clockParams);
    clockParams.startFlag = true;
    clockParams.period = 1;
    Clock_construct(&g_controlClock, (Clock_FuncPtr)motorControlClock, 1, &clockParams);

    // Calculate max -/+ duty cycle change to stay within accel/decel specification
    float minAccelMillis = (float)DRV832X_MAX_MAX_RPM / DRV832X_MAX_ACCELERATION_RPM * 1000.0f;
    float minDecelMillis = (float)DRV832X_MAX_MAX_RPM / DRV832X_MAX_DECELERATION_RPM * 1000.0f;
    g_maxDutyIncrease = g_config.pwmPeriod / minAccelMillis;
    g_maxDutyDecrease = g_config.pwmPeriod / minDecelMillis;

    // Pre-calculate phase lookup for different hall sensor states
    initMotorPhaseTable();

    // Initialize speed sensing sampler
    TimeSampler_init(&g_hallEdgeTimer);

    // Initialize PI motor controller
    PIController_init(&g_controller, DRV832X_CONTROLLER_GAIN_P, DRV832X_CONTROLLER_GAIN_I);

    // Set up GPIO hwi for speed sensing.
    // We just want to detect all edges on the hall sensor signals and measure the time between them
    GPIO_setCallback(pConfig->pin.hallA, edgeInterruptHandler);
    GPIO_setCallback(pConfig->pin.hallB, edgeInterruptHandler);
    GPIO_setCallback(pConfig->pin.hallC, edgeInterruptHandler);
    GPIO_enableInt(pConfig->pin.hallA);
    GPIO_enableInt(pConfig->pin.hallB);
    GPIO_enableInt(pConfig->pin.hallC);

    g_currentMotorPhase = pollMotorPhase();
    g_motorState = MotorControl_State_Unknown;

    // Initialize state machine
    initStateInterfaces();

    // Transition to the Idle state
    return MotorControl_transition(MotorControl_State_Idle);
}

void MotorControl_setSpeed(uint32_t rpm)
{
    g_targetRpm = rpm;
}

bool MotorControl_start()
{
    return MotorControl_transition(MotorControl_State_Starting);
}

bool MotorControl_stop()
{
    return MotorControl_transition(MotorControl_State_Stopping);
}

uint32_t MotorControl_getSpeed()
{
    return (uint32_t)g_avgRpm;
}

void MotorControl_estop()
{
    MotorControl_transition(MotorControl_State_EStopping);
}

MotorControl_State MotorControl_getState()
{
    return g_motorState;
}

static void applyMotorControlAction(uint32_t targetRpm, uint32_t speed, uint32_t tick)
{
    PIController_update(&g_controller, targetRpm, speed, tick);

    float controlAction = PIController_getSignal(&g_controller) * DRV832X_CONTROLLER_GAIN;

    if (controlAction < -g_maxDutyDecrease)
        controlAction = -g_maxDutyDecrease;
    if (controlAction > g_maxDutyIncrease)
        controlAction = g_maxDutyIncrease;

    g_currentDutyCycle += controlAction;
    g_currentDutyCycle = g_currentDutyCycle < 0 ? 0.0f: g_currentDutyCycle;
    g_currentDutyCycle = g_currentDutyCycle > g_config.pwmPeriod ? g_config.pwmPeriod : g_currentDutyCycle;

    setDuty((uint32_t)g_currentDutyCycle);
    setMotorPhase(g_currentMotorPhase + 1);
}

// MOTOR CONTROL STATE MACHINE IMPLEMENTATION BELOW

//----------------------------------------------
// Idle State implementation
static bool idle_canTransition(MotorControl_State newState)
{
    // Idle can transition to starting
    return newState == MotorControl_State_Starting;
}

static void idle_activate()
{
    disableMotor();
    g_currentDutyCycle = 0;
}
//----------------------------------------------

//----------------------------------------------
// Starting State implementation
static bool starting_canTransition(MotorControl_State newState)
{
    // Starting can transition to running and estop
    return newState == MotorControl_State_EStopping
      || newState == MotorControl_State_Running;
}

static void starting_activate()
{
    enableMotor();
    // No integral while accelerating
    PIController_reset(&g_controller, DRV832X_CONTROLLER_GAIN_P, 0);
    g_currentMotorPhase = pollMotorPhase();
}

static void starting_onMotorUpdate(uint32_t speed, uint32_t tick)
{
    applyMotorControlAction(g_targetRpm, speed, tick);

    if (abs(g_targetRpm - speed) < 500)
        MotorControl_transition(MotorControl_State_Running);
}
//----------------------------------------------

//----------------------------------------------
// Running State implementation
static bool running_canTransition(MotorControl_State newState)
{
    return newState == MotorControl_State_EStopping
      || newState == MotorControl_State_Stopping;
}

static void running_activate()
{
    // Integral when running
    PIController_reset(&g_controller, DRV832X_CONTROLLER_GAIN_P, DRV832X_CONTROLLER_GAIN_I);
}

static void running_onMotorUpdate(uint32_t speed, uint32_t tick)
{
    applyMotorControlAction(g_targetRpm, speed, tick);
}

//----------------------------------------------

//----------------------------------------------
// Stopping State implementation
static bool stopping_canTransition(MotorControl_State newState)
{
    // Stop can only transition from Running
    return newState == MotorControl_State_Idle;
}

static void stopping_onMotorUpdate(uint32_t speed, uint32_t tick)
{
    if (speed == 0)
        MotorControl_transition(MotorControl_State_Idle);
    else
        applyMotorControlAction(0, speed, tick);
}
//----------------------------------------------

//----------------------------------------------
// EStop State implementation
static bool estop_canTransition(MotorControl_State newState)
{
    // EStop can only transition to Idle
    return newState == MotorControl_State_Idle;
}

static void estop_activate()
{
    // Stop motor by setting all phases to high (active break).
    setDuty(20);
    stopMotor(true);
}

static void estop_onMotorUpdate(uint32_t speed, uint32_t tick)
{
    if (speed == 0)
        MotorControl_transition(MotorControl_State_Idle);
}

//----------------------------------------------

static void initStateInterfaces()
{
    // Reset states array
    memset(g_states, 0, sizeof(g_states));

    { // Configure MotorControl_State_Idle state
        IStateController *pState = g_states + MotorControl_State_Idle;
        pState->activate      = idle_activate;
        pState->canTransition = idle_canTransition;
    }
    
    { // Configure MotorControl_State_Starting state
        IStateController *pState = g_states + MotorControl_State_Starting;
        pState->activate      = starting_activate;
        pState->canTransition = starting_canTransition;
        pState->onUpdateMotor = starting_onMotorUpdate;
    }
    
    { // Configure MotorControl_State_Running state
        IStateController *pState = g_states + MotorControl_State_Running;
        pState->canTransition = running_canTransition;
        pState->onUpdateMotor = running_onMotorUpdate;
        pState->activate      = running_activate;
    }

    { // Configure MotorControl_State_Stopping state
        IStateController *pState = g_states + MotorControl_State_Stopping;
        pState->canTransition = stopping_canTransition;
        pState->onUpdateMotor = stopping_onMotorUpdate;
    }
    
    { // Configure MotorControl_State_EStopping state
        IStateController *pState = g_states + MotorControl_State_EStopping;
        pState->activate      = estop_activate;
        pState->canTransition = estop_canTransition;
        pState->onUpdateMotor = estop_onMotorUpdate;
    }
}
