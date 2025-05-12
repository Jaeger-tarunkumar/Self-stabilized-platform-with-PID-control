/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo myservo1;  // create Servo object to control a servo
Servo myservo2;
// twelve Servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo1.attach(9);  // attaches the servo on pin 9 to the Servo object
  myservo2.attach(10);
  for (pos = 0; pos <= 79; pos += 1) { 
    myservo1.write(pos);              
    delay(15);   
  }
  for (pos = 0; pos <= 88; pos += 1) { 
    myservo2.write(pos);              
    delay(15);   
  }
}

void loop() {
 
}
