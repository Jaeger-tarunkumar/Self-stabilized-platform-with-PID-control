#include <Wire.h>
#include <MPU6050_tockn.h>
#include <Servo.h>

MPU6050 mpu6050(Wire);
Servo servo;
Servo servo1;

float angleY = 90.0;
float angleX = 90.0;


void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);  // Calibrate gyro with Serial Monitor open

  servo.attach(9);
  servo1.attach(10);
  servo.write(angleY);
  servo1.write(angleX);
  angleY = mpu6050.getAngleY();

}

void loop() {
  mpu6050.update();
  float currentTiltY = mpu6050.getAngleY();  // -90 to +90 deg
  float currentTiltX = mpu6050.getAngleX();
  float servoAngleY = 90 + currentTiltY;  
  float servoAngleX = 90 + currentTiltX ;    // 90 = upright
  

  // Clamp for servo safety
  servoAngleY = constrain(servoAngleY, 0, 180);
  servoAngleX = constrain(servoAngleX, 0, 180);


  servo.write(servoAngleY);
  servo1.write(servoAngleX);

  Serial.print("Current Tilt Y: ");
  Serial.print(currentTiltY);
  Serial.print("Current Tilt X: ");
  Serial.print(currentTiltX);
  Serial.print(" -> Servo Angle Y: ");
  Serial.println(servoAngleY);
  Serial.print(" -> Servo Angle X: ");
  Serial.println(servoAngleX);

  delay(5);
}


