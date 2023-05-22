# DRV8323 Motor Driver

## Motor Control

the DRV832x motor driver board should include the following functionalities:
  1. Start and safely control the motor, handling any fault conditions, by using the provided state transition diagram (see reference document). The API should allow you to control the phases of the motor, as described in the supporting documents.

  2. Control the motor’s speed in Revolutions Per Minute (RPM), based on user input.

  3. Safely start and accelerate the motor to the desired speed (in RPM), following the motor acceleration specifications provided below. Your API should ensure that the motor accelerates safely.

  4. Safely decelerate the motor, adhering to the motor deceleration specifications provided below. Your API should ensure that the motor slows down and stops safely.

To emulate the driver of a vehicle pressing the foot pedal, the motor must be controlled to match a user-modified desired speed or RPM. This requires measuring the motor’s speed and adjusting the motor power (PWM duty cycle) to match the desired speed. However, the actual motor speed may fluctuate depending on the motor load, and is not constant for a constant PWM duty value. Therefore, it’s essential to measure the motor’s speed to control its speed accurately (i.e., using a closed-loop controller).

For safe vehicle (motor) starting and braking, the system must regulate the motor’s speed according to acceleration and deceleration limits. Please refer to the separate reference
document for the speed control diagram. Additionally, you will need to explore how to initiate a 3-phase brushless motor correctly. To help set up the GPIO pins and drive the brushless motor’s phases accurately, a third-party motor driver library (MotorLib) will be
provided.

The system will also require different deceleration limits based on whether an emergency condition has occurred or the user has inputted a slower desired speed. The acceleration and deceleration limits are defined as follows:
  1. Motor Acceleration ≤ 500RPM/s
  2. Motor Deceleration (setting slower speed) ≤ 500RPM/s
  3. Motor Deceleration (emergency stop aka e-stop) = 1000RPM/s

Please ensure you handle units correctly, as acceleration and deceleration values will be tested. Note, the deceleration for e-stop should be equal to (not greater or less than) the desired value as we would like the vehicle to come to a complete stop quickly but safely.

## E-Stop

The system should respond appropriately when certain sensor thresholds indicate an emergency condition for the vehicle. The following conditions should trigger an emergency stop
(e-stop) for the motor control, causing the motor to safely brake using the previously indicated deceleration limit. Please note that you should only implement the temperature,
vehicle acceleration, and distance conditions if you have selected the sensor in the relevant
section 2.2.
  
  1. Total motor current has exceeded a user-defined threshold indicating the motor has a fault
  
  2. Temperature of motor has exceeded a user-defined threshold indicating the motor is overheating (see section 2.2).

  3. Absolute vehicle acceleration measured from the IMU (not motor acceleration) has exceeded a user-defined threshold indicating an impact has occurred (see section 2.2).

  4. Distance has gone below a user defined threshold (the vehicle is about to hit an obstacle) measured via the Time of Flight distance sensor (see section 2.2)

## Motor Power Sensor

The Power Sensing driver is responsible for accurately reading and filtering the two motor phase currents using the analogue signals provided by the current sensors on the DRV8323 board. It’s worth noting that only two out of the three phases are available for ADC measurement, so you will need to estimate the total power consumption based on the two measured phases. To calculate the power usage of the motor, you should use the measured motor current and nominal motor voltage of 24V. It is crucial to filter the current readings to eliminate any noise and ensure accurate power measurements.

## Speed Sensor

The Speed Sensing driver should measure and filter the motor’s current speed in Revolutions Per Minute (RPM) for the system to match a desired speed/RPM modified by the user interface. Speed-sensing can be achieved by measuring the time between the edges of the motor’s hall effect lines (Hall A, B & C).
