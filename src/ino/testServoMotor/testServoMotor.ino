#include <Servo.h>
#include <elapsedMillis.h>

#define PINSERVO1 9

elapsedMillis timerGlobal;
const int timeStep = 5; // ms

Servo myServo;
int angleServo;
int stepServo;

void setup() {
  // Servo Setup
  myServo.attach(PINSERVO1);
}

void loop() {
  if( timerGlobal > timeStep ) // Control Frequency time
  {
    // Reset Timer
    timerGlobal = 0; 

    myServo.write(angleServo); 
    stepServo = (angleServo >=180)?-1:(angleServo <= 0) ?1:stepServo;
    angleServo += stepServo;

    // 
    Serial.print(timerGlobal);
    Serial.print(" ");
    Serial.println(angleServo);
  }

}
