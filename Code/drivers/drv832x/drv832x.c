#include "drv832x.h"
#include "motorlib/motorlib.h"
#include "detail/TimeSampler.h"
#include "detail/PIController.h"
#include "Board.h"

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/GPIO.h>

#include <float.h>


drv832x_Config g_config;

volatile float g_currentDutyCycle = 0.0f;

// Current speed of the motor
volatile uint32_t g_currentSpeed = 0;

// Flag that is set to request an e-stop
volatile bool g_estop = false;

// Current state of the motor driver
volatile drv832x_State g_motorState = drv832x_Unknown;

volatile uint32_t g_currentMotorPhase = 0;

// LUT used to get the phase from the sensor states
volatile uint8_t g_phaseLUT[8] = {};

/// Responsible for calculating time between hall sensor signal edges.
TimeSampler g_hallEdgeTimer;

PIController g_controller;

uint8_t calcPhaseTableIndex(bool a, bool b, bool c) {
    return ((a & 0b1) << 0)
            | ((b & 0b1) << 1)
            | ((c & 0b1) << 2);
}

void initMotorPhaseTable() {
    g_phaseLUT[calcPhaseTableIndex(1, 0, 1)] = 0;
    g_phaseLUT[calcPhaseTableIndex(1, 0, 0)] = 1;
    g_phaseLUT[calcPhaseTableIndex(1, 1, 0)] = 2;
    g_phaseLUT[calcPhaseTableIndex(0, 1, 0)] = 3;
    g_phaseLUT[calcPhaseTableIndex(0, 1, 1)] = 4;
    g_phaseLUT[calcPhaseTableIndex(0, 0, 1)] = 5;
}

uint8_t getMotorPhase(bool a, bool b, bool c) {
    return g_phaseLUT[calcPhaseTableIndex(a, b, c)];
}

//----------------------
// Motor controller task

void setMotorPhase(uint32_t phase) {
    updateMotor(
            (phase) < 3,
            ((phase - 2) % 6) < 3,
            ((phase - 4) % 6) < 3);
}

uint32_t pollMotorPhase() {
    bool stateA = GPIO_read(g_config.pin.hallA);
    bool stateB = GPIO_read(g_config.pin.hallB);
    bool stateC = GPIO_read(g_config.pin.hallC);

    return getMotorPhase(stateA, stateB, stateC);
}

Clock_Struct g_controlClock;
static void Driver_motorControlClock(UArg arg0)
{
    const uint32_t currentTick = Clock_getTicks();
    TimeSampler_discardExpiredSamples(&g_hallEdgeTimer, currentTick);
    const uint32_t speed = drv832x_getSpeed();
    PIController_update(&g_controller, speed, currentTick);
    float controlAction = PIController_getSignal(&g_controller) * DRV832X_CONTROLLER_GAIN;
    g_currentDutyCycle += controlAction;
    g_currentDutyCycle = g_currentDutyCycle < 0 ? 0.0f: g_currentDutyCycle;
    g_currentDutyCycle = g_currentDutyCycle > g_config.pwmPeriod ? g_config.pwmPeriod : g_currentDutyCycle;
    setDuty((uint32_t)g_currentDutyCycle);
    setMotorPhase(g_currentMotorPhase + 1);
}

//----------------------

//----------------------
// Motor speed sensing
static void Driver_edgeInterruptHandler(unsigned int pin)
{
    TimeSampler_addSample(&g_hallEdgeTimer, Clock_getTicks());
    g_currentMotorPhase = pollMotorPhase();
    setMotorPhase(g_currentMotorPhase + 1);
}

//----------------------

void drv832x_Config_init(drv832x_Config *pConfig)
{
    pConfig->pin.hallA = Board_MOTOR_HALL_A;
    pConfig->pin.hallB = Board_MOTOR_HALL_B;
    pConfig->pin.hallC = Board_MOTOR_HALL_C;
    pConfig->pwmPeriod = 50;
}

bool drv832x_init(drv832x_Config const * pConfig)
{
    g_config = *pConfig;

    // Pre-calculate phase lookup for different hall sensor states
    initMotorPhaseTable();

    TimeSampler_init(&g_hallEdgeTimer);
    PIController_init(&g_controller, DRV832X_CONTROLLER_GAIN_P, DRV832X_CONTROLLER_GAIN_I);

    /* Construct clock threads */
    Clock_Params clockParams;
    Clock_Params_init(&clockParams);
    clockParams.startFlag = true;
    clockParams.period = 1;
    Clock_construct(&g_controlClock, (Clock_FuncPtr)Driver_motorControlClock, 1, &clockParams);

    // Set up GPIO hwi for speed sensing.
    // We just want to detect all edges on the hall sensor signals and measure the time between them
    GPIO_setCallback(pConfig->pin.hallA, Driver_edgeInterruptHandler);
    GPIO_setCallback(pConfig->pin.hallB, Driver_edgeInterruptHandler);
    GPIO_setCallback(pConfig->pin.hallC, Driver_edgeInterruptHandler);

    GPIO_enableInt(pConfig->pin.hallA);
    GPIO_enableInt(pConfig->pin.hallB);
    GPIO_enableInt(pConfig->pin.hallC);

    Error_Block err;
    if (!initMotorLib(pConfig->pwmPeriod, &err))
    {
        System_printf(err.msg);
        System_flush();
        // TODO: Output error
        return false;
    }

    g_currentMotorPhase = pollMotorPhase();

    enableMotor();

    return true;
}

void drv832x_setSpeed(uint32_t rpm)
{
    PIController_setTarget(&g_controller, rpm);
}

bool drv832x_start()
{
    // To start we need to transition the motor to the drv832x_Starting state.
    // This is only valid if the motor is currently in the drv832x_Idle state
    if (g_motorState != drv832x_Idle)
        return false;

    g_motorState = drv832x_Starting;
    // TODO: Signal semaphore to wake up the driver task
    return true;
}

void drv832x_stop()
{
    drv832x_setSpeed(0); // Set target speed to 0 to stop the motor.
    PIController_reset(&g_controller);
}

uint32_t drv832x_getSpeed()
{
    const uint32_t ticksPerSecond = 1000000 / Clock_tickPeriod;

    float avgEdgeTicks = TimeSampler_calculateSpeed(&g_hallEdgeTimer);
    float ticksPerRotation = DRV832X_HALLEFFECT_EDGES_PER_ROTATION * avgEdgeTicks;
    if (ticksPerRotation == 0)
        return 0;

    return (uint32_t)((ticksPerSecond * 60) / ticksPerRotation); // Convert from ticks-per-rotation to RPM
}

void drv832x_estop()
{
    g_estop = true;
}

drv832x_State drv832x_getState()
{
    return g_motorState;
}

