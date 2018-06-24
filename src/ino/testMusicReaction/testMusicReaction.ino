
// Libraries ------------------------------------------------
#include <elapsedMillis.h>
#include <Adafruit_NeoPixel.h>

// Constants ------------------------------------------------
#define PINMIC1 A0
#define PINMIC2 A1
#define PINMIC2THRESH 42
#define PINLED 13
#define PINCITY 2
#define PINBASE 5
#define PINDJ 3

#define SIZE_BUFFER 12

// Global Variables -----------------------------------------
elapsedMicros timerGlobal;
const int timeStep = 1000;

int ledState;
float a0;
float musicInput;

int k;
int bufferRawMic1[SIZE_BUFFER];
int bufferRawMic2[SIZE_BUFFER];
int bufferFilterBass[SIZE_BUFFER];
const float cheb1Coef[] = {
  0.5771, 1.1542 , 0.5771 ,1.0000, 0.6534, 0.6818,
  0.2688, 0.5375 , 0.2688 ,1.0000, -0.0402, 0.1276};

// Main Functions --------------------------------------------
void setup() {
  
  Serial.begin(115200);

  pinMode( PINLED, OUTPUT );
  ledState = LOW;

  // Calculate High pass filter (2/timeStep)
  float freqCut0 = 0.3; // Khz
  float w0 = TWO_PI*timeStep*freqCut0;
  a0 = w0/(w0+1);

  Serial.print("Filters PARAM: ");
  Serial.println(a0);

  k = 0;

  delay(500);
  timerGlobal = 0;
}

void loop() {
  if( timerGlobal > timeStep ) // Control Frequency time
  {
    timerGlobal = 0;
    
    // Read Input
    int rawMic1 = analogRead(PINMIC1);
    int rawMic2 = analogRead(PINMIC2);
    int rawMic2Detect = digitalRead(PINMIC2THRESH);
    
    bufferRawMic1[k] = rawMic1;
    bufferRawMic2[k] = rawMic2;

    // FIR Cheb Filter
    bufferFilterBass[k] = 0;
    for(int i = 0;i < k;i++)
    {
       bufferFilterBass[k] += cheb1Coef[k-i]*bufferRawMic1[i];
    }
    for(int i = k;i < SIZE_BUFFER;i++)
    {
       bufferFilterBass[k] += cheb1Coef[i-k]*bufferRawMic1[i];
    }

    // Basic Filter
    bufferFilterBass[k] = (rawMic1 + rawMic2)/2;

    // Toggle Led for evaluating time freq
    ledState =! ledState ;
    digitalWrite( PINLED ,ledState);
    digitalWrite( PINCITY ,rawMic2Detect);
    digitalWrite( PINDJ , rawMic2Detect);
    digitalWrite( PINBASE , bufferFilterBass[k] > 545 );

    // Increase Counter
    k = (k<(SIZE_BUFFER-1))?k+1:0;

    /*/ Print Everything for debug
    Serial.print(timerGlobal);
    Serial.print(" ");
    Serial.print(millis());
    Serial.print(" ");
    Serial.print(rawMic1);
    Serial.print(" ");
    Serial.print(rawMic2);
    Serial.print(" ");
    Serial.print(bufferFilterBass[k]);
    Serial.print(" ");
    Serial.print(timerGlobal);
    Serial.println();//*/
  }
}
