#include <Wire.h>
#include <MPU6050_tockn.h>
#include <Servo.h>

MPU6050 mpu6050(Wire);
Servo servoY;  // Bottom axis
Servo servoX;  // Top axis

// Filtered angles
float angleY_filtered = 0;
float angleX_filtered = 0;

float baseY = 79;
float baseX = 88;

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

float preverrorY = 0;
float preverrorX = 0;

float t2 = 0;
float t1 = 0;

float integralY = 0;
float integralX = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);  // Keep device steady during calibration

  servoY.attach(9);
  servoX.attach(10);

  angleY_filtered = 0;
  angleX_filtered = 0;

  servoY.write(baseY);
  servoX.write(baseX);
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
    float deltaTime = 0;
    
    // === OPTION 1: DIRECT MAPPING ===
    //float servoAngleY = 80 + angleY_filtered;
    //loat servoAngleX = 88 + angleX_filtered;

    // === OPTION 2: PROPORTIONAL CONTROL (comment OPTION 1 above if using this) ===
    t1 = millis();
    deltaTime = t2 - t1;
     float Kp = 1.05;
     float Ki = 0.001;
     float Kd = 0.07;
     
     float errorY = 0 - angleY_filtered;  // Target is level
     float errorX = 0 - angleX_filtered;
     integralY += errorY*deltaTime;
     integralX += errorX*deltaTime;
     float dtY = (errorY - preverrorY)/deltaTime;
     float dtX = (errorX - preverrorX)/deltaTime;
     float correctionY = Kp*errorY + Kd*dtY + Ki*integralY;
     float correctionX = Kp*errorX + Kd*dtX + Ki*integralX;
     float maxCorrection = 50.0;  // tune this
     correctionY = constrain(correctionY, -maxCorrection, maxCorrection);
     correctionX = constrain(correctionX, -maxCorrection, maxCorrection);

     float servoAngleY = baseY - (correctionY );
     float servoAngleX = baseX - (correctionX );
     
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
    preverrorY = errorY;
    preverrorX = errorX;
    delay(15);
    t2 = t1;
  }
}
