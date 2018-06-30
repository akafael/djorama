/**
 * Djorama Controler
 * 
 * Features:
 *  - React to Music Volume
 *  - Simple Music Detection
 *  - Independent timer for changing effect, color and speed
 * 
 * @author Rafael Lima
 * @version 0.2
 */

// Libraries -------------------------------------------------------
#include <elapsedMillis.h>
#include <FastLED.h>

// Constants -------------------------------------------------------
#define PINLED 5
#define PINMIC1_ALG A0
#define PINMIC1_DIG 2

#define NUM_LEDS 20
#define SIZECOLORPALET 10
#define NUMEFFECTS 4

#define TIMESTEP_MUSIC 10
#define TIMESTEP_BEAT 10
#define TIMESTEP_LOOP 100
#define TIMESTEP_COLOR 500
#define TIMESTEP_EFFECT 20000

#define SOUND_BASS_THRESHOLD 570 // Max 600

// Global Variables ------------------------------------------------

// Timers
elapsedMillis timerSilence;
elapsedMillis timerLEDStrip;
elapsedMillis timerLoop;
elapsedMillis timerBeat;
elapsedMillis timerMusicInput;
elapsedMillis timerColor;
elapsedMillis timerEffect;

// LED Strip
CRGB leds[NUM_LEDS];
const CRGB colorPalet[] = {0xF7F7F7, // White Smoke
                           0xBA0034, // Crimson Glory
                           0xF70035, // Carmine Red
                           0xFA07F2,
                           0xDD49B8, // Pink (PANTONE)
                           0xF7F7F7, // White Smoke
                           0x00BAAD, // Amazonite
                           0x00F7E6, // Tuorquoise Blue
                           0xFA07F2, 
                           0xDD49B8};// Pink (PANTONE)

// Index and aux variables
unsigned int indexEffect = 0;
unsigned int indexColor = 0;
unsigned int i = 0;
int inc;
int isBeat;
int isBass;
int isMusicPlaying = 0;

int timeStepLedTransition = TIMESTEP_LOOP;

int soundSilenceRef = 0;

// Function Pointer Vector for easy effect access
void (*effectVector[NUMEFFECTS])(unsigned int k,CRGB color);

// Main functions --------------------------------------------------
void setup() {
  // Setup Pins
  pinMode(PINMIC1_ALG,INPUT);
  pinMode(PINMIC1_DIG,INPUT);
  
  // Start LED Strip
  FastLED.addLeds<WS2811, PINLED, BRG>(leds, NUM_LEDS);

  // Reset all index
  indexEffect = 0;
  indexColor = 0;
  i = 0;

  // Register effects used
  effectVector[0] = effectLightMoving;
  effectVector[1] = effectLightFill;
  effectVector[2] = effectLightDots;
  effectVector[3] = effectLightSideFill;

  // Reset Timers
}

void loop() {

  // Read Mic and set treshold triggers
  int musicInput = analogRead(PINMIC1_ALG);   // Raw Input
  isBeat = digitalRead(PINMIC1_DIG);          // Potentiometer Threshold
  isBass = musicInput > SOUND_BASS_THRESHOLD; // Hardcode Threshold
  
  // Select Color Timer
  if( timerColor > TIMESTEP_COLOR)
  {
    timerColor = 0; // Reset Timer
    indexColor = (indexColor<9)? indexColor +1 : 0;
  }

  // Select Effect Timer
  if( timerEffect > TIMESTEP_EFFECT)
  {
    timerEffect = 0; // Reset Timer
    indexEffect = (indexEffect < 3)? indexEffect +1 : 1;
  }

  // Light Effects Timer
  if( timerLoop > timeStepLedTransition ) // Control Frequency time
  {
    timerLoop = 0; // Reset Timer

    // Alternate from 0 to NUM_LEDS
    if( i >= NUM_LEDS ) inc = -1;
    if( i <= 0) inc = 1;
    i += inc;
  }

  // Change Light on Beat
  if(isBeat)
  {
    if(timerSilence > 10)
    {
      timeStepLedTransition = 100;
    }
    else if(timerSilence > 5)
    {
      timeStepLedTransition = 100/timerSilence; // Speed Up transition
    }

    effectVector[indexEffect](i,colorPalet[indexColor]);
    
    timerSilence = 0;
    FastLED.show();
  }
  else if(timerSilence > 200)
  {
    soundSilenceRef = musicInput;
    fill_solid(leds,NUM_LEDS,0x000000);
    FastLED.show();
  }
}

// Auxilar Functions -------------------------------------------------

/**
 * Effect: Turn On all LEDs
 */
void effectTurnOn(unsigned int k,CRGB color)
{
    fill_solid(leds,NUM_LEDS,color);
}

/**
 * Effect: move LED on position
 */
void effectLightMoving(unsigned int k,CRGB color)
{
    fill_solid(leds,NUM_LEDS,0x000000);
    leds[k] = color;
}

/**
 * Effect: Turn On LEDS in sequence
 */
void effectLightFill(unsigned int k,CRGB color)
{
    fill_solid(leds,NUM_LEDS,0x000000);
    fill_solid(leds,i,color);
}

/**
 * Effect: move LEDs position starting from sides and 
 *         turn on everything when they reach the center
 */
void effectLightCollision(unsigned int k,CRGB color)
{
    if( (NUM_LEDS - k-k) <= 1) // Center
    {
      fill_solid(leds,NUM_LEDS,color);
    }
    else
    {
      fill_solid(leds,NUM_LEDS,0x000000);
      leds[k] = color;
      leds[NUM_LEDS-k] = color;
    }
}

/**
 * Effect: move LEDs position starting from sides and 
 *         turn on everything when they reach the center
 */
void effectLightDots(unsigned int k,CRGB color)
{
    fill_solid(leds,NUM_LEDS,0x000000);
    leds[k] = color;
    leds[NUM_LEDS-k] = color;
}

/**
 * Effect: move LEDs position starting from sides and 
 *         turn on everything when they reach the center
 */
void effectLightSideFill(unsigned int k,CRGB color)
{
    int endK = (NUM_LEDS - k);
    if( endK > k) // Center
    {
      fill_solid(leds,NUM_LEDS,0x000000);
      fill_solid(leds,k,color);
      fill_solid(leds+endK,k,color);
    }
    else
    {
      fill_solid(leds,NUM_LEDS,color);
      fill_solid(leds+endK,k-endK,0x000000);
    }
}
