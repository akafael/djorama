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
#define PINLED 9

#define NUM_LEDS 16
#define SIZECOLORPALET 10
#define NUMEFFECTS 7

#define TIMESTEP_LOOP 200
#define TIMESTEP_COLOR 1000
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
void (*effectVector[NUMEFFECTS])(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color);

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
  effectVector[2] = effectLightCollision;
  effectVector[3] = effectLightDots;
  effectVector[4] = effectLightSideFill;
  effectVector[5] = effectBlockMove;
  effectVector[6] = effectAlternate;

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
    else if( i <= 0) inc = 1;
    i += inc;

    // Effect Step
    //effectVector[indexEffect](leds,NUM_LEDS,i,colorPalet[indexColor]);
    fill_solid(leds,NUM_LEDS,0x0000FF);
    //effectTurnOn(i,colorPalet[indexColor]); // ignore effect and turn Everything off
    FastLED.show();

    digitalWrite( LED_BUILTIN, !digitalRead( LED_BUILTIN ) ); // blink buildin led
  }
}

// Auxilar Functions -------------------------------------------------

/**
 * Effect: Turn On all LEDs
 */
void effectTurnOn(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    fill_solid(ledStrip,stripSize,color);
}

/**
 * Effect: move LED on position
 */
void effectLightMoving(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    fill_solid(ledStrip,stripSize,0x000000);
    ledStrip[k] = color;
}

/**
 * Effect: Turn On LEDS in sequence
 */
void effectLightFill(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    fill_solid(ledStrip,stripSize,0x000000);
    fill_solid(ledStrip,k,color);
}

/**
 * Effect: move LEDs position start to side and 
 *         turn on everything when they reach the center
 */
void effectLightCollision(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    if( (stripSize - k-k) <= 1)
    {
      fill_solid(ledStrip,stripSize,color);
    }
    else
    {
      fill_solid(ledStrip,stripSize,0x000000);
      ledStrip[k] = color;
      ledStrip[stripSize-k] = color;
    }
}

/**
 * Effect: move LEDs position starting from sides and 
 *         turn on everything when they reach the center
 */
void effectLightDots(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    fill_solid(ledStrip,stripSize,0x000000);
    ledStrip[k] = color;
    ledStrip[stripSize-k] = color;
}

/**
 * Effect: move LEDs position starting from sides and 
 *         turn on everything when they reach the center
 */
void effectLightSideFill(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    int endK = (stripSize - k);
    if( endK > k) // Center
    {
      fill_solid(ledStrip,stripSize,0x000000);
      fill_solid(ledStrip,k,color);
      fill_solid(ledStrip+endK,k,color);
      fill_solid(ledStrip,stripSize,0x000000);
    }
    else
    {
      fill_solid(ledStrip,stripSize,color);
      fill_solid(ledStrip+endK,k-endK,0x000000);
    }
}

/**
 * Effect: Light a segment
 */
void effectBlockMove(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    int blockSize = stripSize/4;
    int blockNum = k%4;
    int blockPos = blockNum*blockSize;

    fill_solid(ledStrip,stripSize,0x000000);
    fill_solid(ledStrip+blockPos,blockSize,color);
}

/**
 * Effect: Alternate between Odds and Even Count
 */
void effectAlternate(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    int hitOdds = k%(stripSize/2);
    fill_solid(ledStrip,stripSize,0x000000);
    fill_solid(ledStrip+2*k+hitOdds,1,color);
}

/**
 * Effect: Light a segment
 */
void effectBlockRotate(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    int blockSize = stripSize/4;
    int blockNum = k%4;

    fill_solid(ledStrip,stripSize,0x000000);
    for(int blockPos = 0; blockPos < 4; blockPos++)
      ledStrip[blockPos*blockSize+blockNum] = color;
}

/**
 * Effect: Light a segment
 */
void effectBlockX(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    int blockSize = stripSize/4;
    int lightA = k%4;
    int lightB = (k+2)%4;  

    fill_solid(ledStrip,stripSize,0x000000);
    for(int blockPos = 0; blockPos < 4; blockPos++)
    {
      ledStrip[blockPos*blockSize+lightA] = color;
      ledStrip[blockPos*blockSize+lightB] = color;  
    }
}

/**
 * Effect: Turn On all LEDs
 */
void effectBlink(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
{
    //fill_solid(ledStrip,stripSize,color*(k%2));
}
