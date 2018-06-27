/**
 * LED Strip Controler
 * 
 * Features:
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

#define NUM_LEDS 20
#define SIZECOLORPALET 10
#define NUMEFFECTS 10

#define TIMESTEP_LOOP 100
#define TIMESTEP_COLOR 500
#define TIMESTEP_EFFECT 5000

// Global Variables ------------------------------------------------

// Timers
elapsedMillis timerLoop;
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

// Function Pointer Vector for easy effect access
void (*effectVector[NUMEFFECTS])(unsigned int k,CRGB color);

// Main functions --------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2811, PINLED, BRG>(leds, NUM_LEDS);

  // Reset all index
  indexEffect = 0;
  indexColor = 0;
  i = 0;

  // Register effects used
  effectVector[0] = effectTurnOn;
  effectVector[1] = effectLightMoving;
  effectVector[2] = effectLightFill;
  effectVector[3] = effectLightCollision;

  // Reset Timers
}

void loop() {
  // Select Color
  if( timerColor > TIMESTEP_COLOR)
  {
    timerColor = 0; // Reset Timer
    indexColor = (indexColor<9)? indexColor +1 : 0;
  }

  // Select Effect
  if( timerEffect > TIMESTEP_EFFECT)
  {
    timerEffect = 0; // Reset Timer
    indexEffect = (indexEffect<9)? indexEffect +1 : 0;
  }

  // Run Effects
  if( timerLoop > TIMESTEP_LOOP ) // Control Frequency time
  {
    timerLoop = 0; // Reset Timer

    // Alternate from 0 to NUM_LEDS
    if( i >= NUM_LEDS ) inc = -1;
    if( i <= 0) inc = 1;
    i += inc;

    // Effect Step
    effectVector[indexEffect](i,colorPalet[indexColor]);
    //fill_solid(leds,NUM_LEDS,0); // ignore effect and turn Everything off
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
 * Effect: move LEDs position start to side and 
 *         turn on everything when they reach the center
 */
void effectLightCollision(unsigned int k,CRGB color)
{
    if( (NUM_LEDS - k-k) <= 1)
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