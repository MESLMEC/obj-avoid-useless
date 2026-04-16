#include <Arduino.h>
#include <Servo.h>

#define FrontServoPin 10
#define EchoPin 8
#define TrigPin 9
#define DistanceThreshold 25.0F;

const uint8_t rightMotorPins[2] = {4, 5}; // {DigitalPin, PWMPin}
const uint8_t leftMotorPins[2] = {6, 7}; // {PWMpin, DigitalPin}
Servo frontServo = Servo();

bool objectInFront = false;
bool objectInLeft = false;
bool objectInRight = false;

void setupSensors()
{
    pinMode(TrigPin, OUTPUT);
    pinMode(EchoPin, INPUT);
    frontServo.attach(FrontServoPin);
    frontServo.write(90);
}

int getDistance()
{
    digitalWrite(TrigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(TrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(TrigPin, LOW);

    long duration = pulseIn(EchoPin, HIGH);
    return (duration * .0343) / 2; // distance in centimeter
}

void getStatus(float threshold, bool *leftStatusAddress, bool *rightStatusAddress)
{
    // left
    frontServo.write(180);
    delay(1000);
    float leftDistance = getDistance();
    // right
    frontServo.write(0);
    delay(1000);
    float rightDistance = getDistance();
    // reset position
    frontServo.write(90);
    delay(1000);

    *leftStatusAddress = leftDistance < threshold;
    *rightStatusAddress = rightDistance < threshold;
}

void setupMotors()
{
    pinMode(leftMotorPins[0], OUTPUT);
    pinMode(leftMotorPins[1], OUTPUT);
    pinMode(rightMotorPins[0], OUTPUT);
    pinMode(rightMotorPins[1], OUTPUT);
}

void stop()
{
    digitalWrite(leftMotorPins[0], 0);
    digitalWrite(leftMotorPins[1], 0);
    digitalWrite(rightMotorPins[0], 0);
    digitalWrite(rightMotorPins[1], 0);
}

void forward()
{
    digitalWrite(leftMotorPins[0], 0);
    digitalWrite(leftMotorPins[1], 1);
    digitalWrite(rightMotorPins[0], 0);
    digitalWrite(rightMotorPins[1], 1);
}

void left()
{
    digitalWrite(leftMotorPins[0], 1);
    digitalWrite(leftMotorPins[1], 0);
    digitalWrite(rightMotorPins[0], 0);
    digitalWrite(rightMotorPins[1], 1);
}

void right()
{
    digitalWrite(leftMotorPins[0], 0);
    digitalWrite(leftMotorPins[1], 1);
    digitalWrite(rightMotorPins[0], 1);
    digitalWrite(rightMotorPins[1], 0);
}

void driveForward(float speed)
{
    uint8_t pwm = UINT8_MAX * speed;
    digitalWrite(rightMotorPins[0], 0);
    analogWrite(rightMotorPins[1], pwm);
    analogWrite(leftMotorPins[0], UINT8_MAX - pwm);
    digitalWrite(leftMotorPins[1], 1);
}

void driveBackward(float speed)
{
    uint8_t pwm = UINT8_MAX * speed;
    digitalWrite(rightMotorPins[0], 1);
    analogWrite(rightMotorPins[1], UINT8_MAX - pwm);
    analogWrite(leftMotorPins[0], pwm);
    digitalWrite(leftMotorPins[1], 0);
}

void drive(float driveValue)
{
    if (driveValue == 0)
    {
        stop();
    }
    else if (driveValue > 0.0)
    {
        driveForward(abs(driveValue));
    }
    else
    {
        driveBackward(abs(driveValue));
    }
}

void setup()
{
    setupSensors();
    setupMotors();
    Serial.begin(9600);
}

// --- main loop ---
void loop()
{

    float distance = min(60.0, max(15.0, getDistance()));
    Serial.println(distance);

    if (distance < 25.0)
    {
        objectInFront = true;
    }

    if (!objectInFront)
    {
        driveForward(0.7);
    }
    else
    {
        stop();
        delay(500);
        getStatus(25.0, &objectInLeft, &objectInRight);

        // default left
        if (!objectInLeft)
        {
            left();
            delay(500);
            stop();
            delay(500);
            // TODO: Don't use sleep. Actually detect if path is cleart in the front.
        }
        else if (!objectInRight)
        {
            right();
            delay(500);
            stop();
            delay(500);
            // TODO: Don't block the the brain.
        }
        else
        {
            // TODO: Actually spin the whole thing and try to return.
            driveBackward(0.6);
            delay(500);
            stop();
            delay(500);
        }

        objectInFront = false;
        objectInLeft = false;
        objectInRight = false;
    }

    delay(10);
}