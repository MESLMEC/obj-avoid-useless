# Obstacle Avoiding Bot
This is a basic example code for an obstacle avoiding bot. 

## Components Used
- HCSR04 sonar sensor
- SG90 servo motor
- yellow motor x 4
- L298N motor driver
- LM2596 Buck converter (DC to DC)

## Framework
Arduino

**Header Files**

`Arduino.h`  
`Servo.h`

## Limitations
- Slow scan due to only one proximity sensor that rotates and reads.
- All contagious motor pins does not support pwm. That forced us to do unnecessary algorithm to drive motors controlling the speed.