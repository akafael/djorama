
// Libraries -------------------------------------------------------
#include <elapsedMillis.h>

// Constants -------------------------------------------------------
#define PORT_MIC A0
#define PORT_LED 13
#define PINMIC1_DIG 9

// Global Variables ------------------------------------------------
elapsedMillis timerSilence;

int micZeroRef = 0;

// Main functions --------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(PORT_LED,OUTPUT);
  pinMode(PORT_MIC,INPUT);
  pinMode(PINMIC1_DIG,INPUT);
}

void loop() {
  int micInput = analogRead(PORT_MIC);
  int micBeat = digitalRead(PINMIC1_DIG);

  if(micBeat)
  {
     timerSilence = 0;
  }
  else if(timerSilence > 1000)
  {
    micZeroRef = micInput;
  }
  
  
  if(micInput > 650)
  {
    digitalWrite(PORT_LED,HIGH);
  }
  else
  {
    digitalWrite(PORT_LED,LOW);
  }

  Serial.print(millis());
  Serial.print(" ");
  Serial.println(micInput-micZeroRef);
}
