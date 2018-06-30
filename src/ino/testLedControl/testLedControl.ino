
#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>

#define PINMIC A0
#define PINLED 13
#define PINCITY 2
#define PINBASE 5
#define PINDJ 3

#define PINSTRIP 6
#define NUMLEDS_STRIP 2

#define BUFFER_SIZE 10

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(NUMLEDS_STRIP, PINSTRIP, NEO_GRB + NEO_KHZ800);

elapsedMillis timerGlobal;
const int timeStep = 10;

int ledState;
float a0,a1,a2;
float musicInput;
float filterLowPich;
float filterTopPich;
float bassInput;
float midInput;

void setup() {
  
  Serial.begin(115200);

  pinMode( PINLED, OUTPUT );
  ledStrip.begin(); // Initializes the NeoPixel library.  
  ledState = LOW;

  // Calculate High pass filter
  float freqCut0 = 0.06; // Khz
  float w0 = TWO_PI*timeStep*freqCut0;
  a0 = 1/(w0+1);

  // Calculate Low pass filter
  float freqCut1 = 0.3; // Khz
  float w1 = TWO_PI*timeStep*freqCut1;
  a1 = w1/(w1+1);

  // Calculate High pass filter
  float freqCut2 = 0.5; // Khz
  float w2 = TWO_PI*timeStep*freqCut2;
  a2 = 1/(w2+1);

  Serial.print("Filters PARAM: ");
  Serial.print(a0);
  Serial.print(" ");
  Serial.print(a1);
  Serial.print(" ");
  Serial.println(a2);

  delay(500);

  timerGlobal = 0;
}

void loop() {
  if( timerGlobal > timeStep ) // Control Frequency time
  {
    // Reset Timer
    timerGlobal = 0; 

    // Read Input
    int rawInput = analogRead(PINMIC);
    
    // High Pass Filter (20hz)
    filterLowPich = a0*(filterLowPich - musicInput + rawInput);
    // High Pass Filter (500hz)
    midInput = a2*(midInput - musicInput + rawInput);
    musicInput = rawInput;

    // Low Pass Filter (300Hz)
    bassInput = a1*(bassInput - rawInput) + rawInput;

    // Toggle Led for evaluating time freq
    ledState =! ledState ;
    digitalWrite( PINLED ,ledState);
    digitalWrite( PINCITY ,rawInput > 620);
    digitalWrite( PINDJ ,filterLowPich*filterLowPich > 50);
    digitalWrite( PINBASE ,bassInput > 540);

    // Print Everything for debug
    Serial.print(timerGlobal);
    Serial.print(" ");
    Serial.print(millis());
    Serial.print(" ");
    Serial.print(musicInput);
    Serial.print(" ");
    Serial.print(filterLowPich);
    Serial.print(" ");
    Serial.print(midInput);
    Serial.print(" ");
    Serial.println(bassInput);//*/
  }
}
