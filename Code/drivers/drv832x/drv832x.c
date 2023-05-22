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

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define TASKSTACKSIZE 512

typedef struct _Driver {
    /// Options the driver was initialised with.
    drv832x_Config config;
    /// Current motor duty cycle
    float dutyCycle;
    /// Current phase of the motor.
    uint32_t motorPhase;
    /// Flag to trigger an estop.
    bool estop;
    /// Responsible for calculating time between hall sensor signal edges.
    TimeSampler hallEdgeTimer;
    /// Responsible for calculating the correct PWM signal.
    PIController speedController;
    /// Current state of the driver.
    drv832x_State state;
} Driver;

static Driver g_driver;

static SensorState g_phaseTable[][HallSensors_Count] = {
    { SensorState_Rising,  SensorState_Low,     SensorState_High    },
    { SensorState_High,    SensorState_Low,     SensorState_Falling },
    { SensorState_High,    SensorState_Rising,  SensorState_Low     },
    { SensorState_Falling, SensorState_High,    SensorState_Low     },
    { SensorState_Low,     SensorState_High,    SensorState_Rising  },
    { SensorState_Low,     SensorState_Falling, SensorState_High    },
};

static bool g_phaseMotorStateTable[][HallSensors_Count] = {
    { false, false, true  },
    { true,  false, true  },
    { true,  false, false },
    { true,  true,  false },
    { false, true,  false },
    { false, true,  true  },
};

static int calcMotorPhase(SensorState a, SensorState b, SensorState c)
{
    int phase = 0;
    for (phase = 0; phase < 6; ++phase)
    {
        if (a == g_phaseTable[phase][HallSensors_A]
            && b == g_phaseTable[phase][HallSensors_B]
            && c == g_phaseTable[phase][HallSensors_C])
            return phase;
    }

    return 0;
}

static int calcMotorPhaseFromInput(bool a, bool b, bool c)
{
    int phase = 0;
    for (phase = 0; phase < 6; ++phase)
    {
        if (a == g_phaseMotorStateTable[phase][HallSensors_A]
            && b == g_phaseMotorStateTable[phase][HallSensors_B]
            && c == g_phaseMotorStateTable[phase][HallSensors_C])
            return phase;
    }

    return 0;
}

static void Driver_setMotorPhase(int phase)
{
    // Lookup for `updateMotor` parameter for each hall sensor
    static bool g_stateLookup[SensorState_Count] = { false, true, false, true };

    // Lookup sensor state for `phase`
    SensorState aState = g_phaseTable[phase][HallSensors_A];
    SensorState bState = g_phaseTable[phase][HallSensors_B];
    SensorState cState = g_phaseTable[phase][HallSensors_C];

    updateMotor(
       g_stateLookup[aState],
       g_stateLookup[bState],
       g_stateLookup[cState]);
}

static void Driver_edgeInterruptHandler(unsigned int pin)
{
    const uint32_t currentTick = Clock_getTicks();

    TimeSampler_addSample(&g_driver.hallEdgeTimer, currentTick);

    // Record current motor states.
    SensorState states[HallSensors_Count];
    int sensor = 0;
    for (; sensor < HallSensors_Count; ++sensor)
    {
        int currentSensor = g_driver.config.hallPin[sensor];
        int state = GPIO_read(currentSensor);
        if (pin == currentSensor)
            states[sensor] = state ? SensorState_Rising : SensorState_Falling;
        else
            states[sensor] = state ? SensorState_High : SensorState_Low;
    }

    g_driver.motorPhase = calcMotorPhase(states[HallSensors_A], states[HallSensors_B], states[HallSensors_C]);

    Driver_setMotorPhase(g_driver.motorPhase);
}

//----------------------
// Motor controller task
static Task_Struct g_motorControlTask;
static Char g_motorControlStack[DRV832X_TASK_STACK_SIZE];
static void Driver_motorControlTask(UArg a0, UArg a1)
{
    uint32_t lastNumLines = 0;
    float lastControlAction = 0.0f;
    while (1)
    {
        Task_sleep(1);
        const uint32_t currentTick = Clock_getTicks();
        TimeSampler_discardExpiredSamples(&g_driver.hallEdgeTimer, currentTick);
        const uint32_t speed = drv832x_getSpeed();

        // Motor controller logic
        switch (g_driver.state)
        {
        case drv832x_Starting:
            if (lastControlAction < 1)
                g_driver.state = drv832x_Running;
        case drv832x_Running:
        {
            // Update motor controller
            PIController_update(&g_driver.speedController, speed, currentTick);

            // Apply control signal
            float controlAction = PIController_getSignal(&g_driver.speedController) * DRV832X_CONTROLLER_GAIN;
            g_driver.dutyCycle += controlAction;
            g_driver.dutyCycle = max(0, min(g_driver.dutyCycle, (float)g_driver.config.pwmPeriod));
            lastControlAction = controlAction;
            
            // Set motor duty cycle
            setDuty((uint32_t)g_driver.dutyCycle);
            Driver_setMotorPhase(g_driver.motorPhase);
            break;
        }
        case drv832x_EStopping:
            setDuty(g_driver.config.pwmPeriod / 2);
            stopMotor(true);
            break;
        case drv832x_Idle:
            PIController_reset(&g_driver.speedController);
            break;
        }

        // Count hall effect edges
        uint32_t numEdgeSamples =  TimeSampler_totalSamples(&g_driver.hallEdgeTimer);
        if (lastNumLines != numEdgeSamples)
        {
            System_printf("Hall-Effect \n", numEdgeSamples);
            lastNumLines = numEdgeSamples;
        }
        
        System_printf("Phase: %u, RPM: %u, Control Action: %u, Transitions: %u\n", g_driver.motorPhase, speed, (int32_t)(lastControlAction * 100), numEdgeSamples);
        System_flush();
    }
}

//----------------------

void drv832x_Config_init(drv832x_Config *pConfig)
{
    pConfig->hallPin[HallSensors_A] = Board_MOTOR_HALL_A;
    pConfig->hallPin[HallSensors_B] = Board_MOTOR_HALL_B;
    pConfig->hallPin[HallSensors_C] = Board_MOTOR_HALL_C;
    pConfig->pwmPeriod = 50;
}

bool drv832x_init(drv832x_Config const * pConfig)
{
    g_driver.estop = false;
    g_driver.motorPhase = 0;
    g_driver.state = drv832x_Idle;
    g_driver.config = *pConfig;
    TimeSampler_init(&g_driver.hallEdgeTimer);
    PIController_init(
            &g_driver.speedController,
            DRV832X_CONTROLLER_GAIN_P,
            DRV832X_CONTROLLER_GAIN_I
    );

    Error_Block err;
    if (!initMotorLib(pConfig->pwmPeriod, &err))
    {
        System_printf(err.msg);
        System_flush();
        return false;
    }

    Task_Params taskParams;
    Task_Params_init(&taskParams);
    taskParams.stackSize = DRV832X_TASK_STACK_SIZE;
    taskParams.stack     = &g_motorControlStack;
    Task_construct(&g_motorControlTask, Driver_motorControlTask, &taskParams, NULL);

    // Set up GPIO hwi for speed sensing.
    // We just want to detect all edges on the hall sensor signals and measure the time between them
    int pin = 0;
    for (; pin < HallSensors_Count; ++pin)
    {
        GPIO_setCallback(pConfig->hallPin[pin], Driver_edgeInterruptHandler);
        GPIO_enableInt(pConfig->hallPin[pin]);
    }

    pin = 0;
    bool pinState[HallSensors_Count];
    for (; pin < HallSensors_Count; ++pin)
        pinState[pin] = GPIO_read(pConfig->hallPin[pin]);

    g_driver.motorPhase = calcMotorPhaseFromInput(pinState[0], pinState[1], pinState[2]);

    setDuty(0);
    Driver_setMotorPhase(g_driver.motorPhase);
    enableMotor();

    return true;
}

void drv832x_setSpeed(uint32_t rpm)
{
    PIController_setTarget(&g_driver.speedController, rpm);
}

bool drv832x_start()
{
    // To start we need to transition the motor to the drv832x_Starting state.
    // This is only valid if the motor is currently in the drv832x_Idle state
    if (g_driver.state != drv832x_Idle)
        return false;

    // Reset the speed controller before starting the motor
    PIController_reset(&g_driver.speedController);

    g_driver.state = drv832x_Starting;

    // TODO: Signal semaphore to wake up the driver task
    return true;
}

void drv832x_stop()
{
    drv832x_setSpeed(0); // Set target speed to 0 to stop the motor.
}

uint32_t drv832x_getSpeed()
{
    const uint32_t ticksPerSecond = 1000000 / Clock_tickPeriod;

    float avgEdgeTicks = TimeSampler_calculateSpeed(&g_driver.hallEdgeTimer);
    float ticksPerRotation = DRV832X_HALLEFFECT_EDGES_PER_ROTATION * avgEdgeTicks;
    if (ticksPerRotation == 0)
        return 0;

    return (uint32_t)((ticksPerSecond * 60) / ticksPerRotation); // Convert from ticks-per-rotation to RPM
}

void drv832x_estop()
{
    g_driver.estop = true;
}

drv832x_State drv832x_getState()
{
    return g_driver.state;
}

