#include <Wire.h>
#include <MPU6050_tockn.h>
#include <Servo.h>

MPU6050 mpu6050(Wire);
Servo servoY;  // Bottom axis
Servo servoX;  // Top axis

// Filtered angles
float angleY_filtered = 0;
float angleX_filtered = 0;

// Filtering parameters
const float alpha = 0.05;  // Low-pass filter smoothing factor

// Servo update control
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 30;  // ms

// Deadband threshold
const float deadband = 1.0;

// Previous servo angles (to compare for deadband)
float prevServoY = 90;
float prevServoX = 90;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);  // Keep device steady during calibration

  servoY.attach(9);
  servoX.attach(10);

  angleY_filtered = 0;
  angleX_filtered = 0;

  servoY.write(90);
  servoX.write(90);
}

void loop() {
  mpu6050.update();

  // Raw angles from MPU6050
  float rawY = mpu6050.getAngleY();  // Tilt side-to-side
  float rawX = mpu6050.getAngleX();  // Tilt front-to-back

  // Apply low-pass filtering
  angleY_filtered = alpha * rawY + (1 - alpha) * angleY_filtered;
  angleX_filtered = alpha * rawX + (1 - alpha) * angleX_filtered;

  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentTime;

    // === OPTION 1: DIRECT MAPPING ===
    float servoAngleY = 90 + angleY_filtered;
    float servoAngleX = 90 - angleX_filtered;

    // === OPTION 2: PROPORTIONAL CONTROL (comment OPTION 1 above if using this) ===
    // float Kp = 1.5;
    // float errorY = 0 - angleY_filtered;  // Target is level
    // float errorX = 0 - angleX_filtered;
    // float servoAngleY = 90 + (Kp * errorY);
    // float servoAngleX = 90 + (Kp * errorX);

    // Clamp angles
    servoAngleY = constrain(servoAngleY, 0, 180);
    servoAngleX = constrain(servoAngleX, 0, 180);

    // Apply deadband and update servo only if angle change is significant
    if (abs(servoAngleY - prevServoY) > deadband) {
      servoY.write(servoAngleY);
      prevServoY = servoAngleY;
    }
    if (abs(servoAngleX - prevServoX) > deadband) {
      servoX.write(servoAngleX);
      prevServoX = servoAngleX;
    }

    // Serial monitor output
    Serial.print("Filtered Y: ");
    Serial.print(angleY_filtered);
    Serial.print("  -> Servo Y: ");
    Serial.print(servoAngleY);

    Serial.print(" | Filtered X: ");
    Serial.print(angleX_filtered);
    Serial.print("  -> Servo X: ");
    Serial.println(servoAngleX);
  }
}
