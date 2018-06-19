
#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>

#define PINMIC A0
#define PINLED 13
#define PINSTRIP 5
#define NUMLEDS_STRIP 2

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(NUMLEDS_STRIP, PINSTRIP, NEO_GRB + NEO_KHZ800);

elapsedMillis timerGlobal;
const int timeStep = 2;

int ledState;
float a0;
float bassInput;
float midInput;

void setup() {

  pinMode( PINLED, OUTPUT );
  ledStrip.begin(); // Initializes the NeoPixel library.  
  ledState = LOW;

  // Calculate filter
  float freqCut = 0.3; // Khz
  float w0 = TWO_PI*timeStep*freqCut;
  a0 = w0/(w0+1);

  timerGlobal = 0;
}

void loop() {
  if( timerGlobal > timeStep ) // Control Frequency time
  {
    // Reset Timer
    timerGlobal = 0; 

    // Read Input
    int musicInput = analogRead(PINMIC);
    
    // Low Pass Filter - Bass (300hz)
    bassInput = a0*(bassInput - musicInput) + musicInput;

    // High Pass Filter - 300hz
    midInput = musicInput - bassInput;

    // Toggle Led for evaluating time freq
    ledState =! ledState ;
    digitalWrite( PINLED ,ledState);
  }
}
