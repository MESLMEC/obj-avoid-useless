#include <Arduino.h>


// Only change when you know what you are doing.
const uint8_t rightMotorPins[3] = { 3, 2, 5 };
const uint8_t leftMotorPins[3] = { 11, 4, 6 };
const uint8_t trigPins[3] = { 7, 9, 12 };
const uint8_t echoPins[3] = { 8, 10, 13 };
uint8_t MAX_PWM = INT8_MAX;

// States
float distances[3] = { 0.0F };
float distancesPrevious[3] = { 0.0F };


/**
 * @brief Brif
 * 
 * @details details
 * @param sensorr pins
 * @return void
 */
void driveMotor(const uint8_t motorPins[3], float driveValue)
{
  if (driveValue == 0.0F) {
    digitalWrite(motorPins[0], 0x0);
    digitalWrite(motorPins[1], 0x0);
    digitalWrite(motorPins[2], 0x0);
  }
  else {
    // driveValue must be in range (-1.0, +1.0)
    driveValue = min(+1.0F, max(-1.0F, driveValue));
    uint8_t pwm = abs(driveValue) * MAX_PWM;
    if (driveValue < 0.0F) {
      digitalWrite(motorPins[0], 0x1);
      digitalWrite(motorPins[1], 0x0);
      analogWrite(motorPins[2], pwm);
    }
    else {
      digitalWrite(motorPins[0], 0x0);
      digitalWrite(motorPins[1], 0x1);
      analogWrite(motorPins[2], pwm);
    }
  }
}

void stopMotors() {
  driveMotor(leftMotorPins, 0.0F);
  driveMotor(rightMotorPins, 0.0F);
}

/**
 * @details -1.0 is most left and +1.0 is most right
 * @param float steerValue
 * @return void
 */
void driveForward(float steerValue) {
  if (steerValue == 0.0F) {
    driveMotor(leftMotorPins, 0.75F);
    driveMotor(rightMotorPins, 0.75F);
  }
  else {
    if (steerValue > 0.0F) {
      driveMotor(rightMotorPins, abs(steerValue));
      driveMotor(leftMotorPins, 1.0F);
    }
    else {
      driveMotor(rightMotorPins, 1.0F);
      driveMotor(leftMotorPins, abs(steerValue));
    }
  }
}

void spin(float spinValue) {
  if (spinValue == 0.0F) {
    stopMotors();
  }
  else if (spinValue > 0.0F) {
    driveMotor(leftMotorPins, (0.5F + abs(spinValue) / 2.0F));
    driveMotor(rightMotorPins, -(0.5F + abs(spinValue) / 2.0F));
  }
  else {
    driveMotor(leftMotorPins, -(0.5F + abs(spinValue) / 2.0F));
    driveMotor(rightMotorPins, (0.5F + abs(spinValue) / 2.0F));
  }
}


float getDistance(uint8_t sensorIndex)
{
  long duration = 0;
  float distance = 0.0F;
  digitalWrite(trigPins[sensorIndex], 0x0);
  delayMicroseconds(2);
  digitalWrite(trigPins[sensorIndex], 0x1);
  delayMicroseconds(10);
  digitalWrite(trigPins[sensorIndex], 0x0);

  duration = pulseIn(echoPins[sensorIndex], 0x1, 6000);
  if (duration == 0) {
    distance = 48.0F;
  }
  else if (duration < 200) {
    distance = 3.0F;
  }
  else {
    distance = (float) duration * 0.0343F / 2.0F;
  }
  return distance - 3.0F;
}


void getDistances() {
  for (uint8_t sensorIndex = 0; sensorIndex < 3; sensorIndex++) {
    distancesPrevious[sensorIndex] = distances[sensorIndex];
    distances[sensorIndex] = getDistance(sensorIndex);
  }
}


void setup()
{
  for (uint8_t pinIndex = 0; pinIndex < 3; pinIndex++) {
    pinMode(leftMotorPins[pinIndex], 0x1);
    pinMode(leftMotorPins[pinIndex], 0x1);
    pinMode(trigPins[pinIndex], 0x1);
    pinMode(echoPins[pinIndex], 0x0);
    digitalWrite(leftMotorPins[pinIndex], 0x0);
    digitalWrite(rightMotorPins[pinIndex], 0x0);
  }
  
  Serial.begin(115200);
}

void loop()
{
  getDistances();
  Serial.print(distances[1]);
  Serial.print("\t");
  float steerValue = min(20.0, distances[2]) - min(20.0, distances[0]); 
  steerValue /= 20.0F;
  Serial.print(steerValue);

  if (distances[1] < 15.0F) {
    spin((steerValue < 0.0F) ? -1.0F : 1.0F);
  }
  else {
    if (abs(steerValue) < 0.5) {
      driveForward(steerValue);
    }
    else {
      spin(steerValue);
    }
  }
  Serial.println();
  delay(10);
}
