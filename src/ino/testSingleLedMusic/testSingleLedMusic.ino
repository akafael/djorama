#include <elapsedMillis.h>

#include "testSingleLedMusic.h"

#define PIN_LED 13

elapsedMicros mTimer;
int i;

void setup() {

  Serial.begin(9600);

  i=0;

  digitalWrite(PIN_LED,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  if(mTimer > 142857) // 7hz
  {
    mTimer = 0;
    int val = music[i];
    i=(i>SAMPLE_NUM)?0:(i+1);

    // Blink Led to Show BMP
    if(val > 70)
    { 
      digitalWrite(PIN_LED,HIGH);
      Serial.println(val);
    }
    else
    {
      digitalWrite(PIN_LED,LOW);
    }

    //Serial.println(val);

  }
  
}
