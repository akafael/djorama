#include <Servo.h>
#include <elapsedMillis.h>

#define PINSERVO1 9
#define PINMIC1_DIG 5

elapsedMillis timerGlobal;
const int timeStep = 5000/180; // ms
const int angleStep = 1; // degres
const int angleRange = 180; // degres

Servo myServo;
int angleServo;
int stepServo;

void setup() {
  // Servo Setup
  myServo.attach(PINSERVO1);
}

void loop() {

  int isBeat = digitalRead(PINMIC1_DIG);


  if(isBeat)
  {
     stepServo = (angleServo >=angleRange)?-angleStep:(angleServo <= 0) ?angleStep:stepServo;
  }


  if( timerGlobal > timeStep ) // Control Frequency time
  {
    // Reset Timer
    timerGlobal = 0; 

    myServo.write(angleServo); 
    angleServo += stepServo;

    // 
    Serial.print(timerGlobal);
    Serial.print(" ");
    Serial.println(angleServo);
  }

}
