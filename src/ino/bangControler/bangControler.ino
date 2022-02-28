/**
 * Bang Controler
 *
 * Features:
 *  - React to Sound Volume
 *  - Music Detection by Silence Time
 *  - Independent timer for changing effect, color and speed
 *  - LED Strip Controled By Music
 *
 * @author Rafael Lima
 * @version 0.4
 */

// Libraries -------------------------------------------------------
#include <elapsedMillis.h>
#include <FastLED.h>
#include "ledStripEffects/ledStripEffects.h"

// Constants -------------------------------------------------------
#define PINLEDS_SPEAKERS     9
#define PINLIDS_BASE         10
#define PINMIC1_ALG          A4
#define PINMIC2_ALG          A0
#define PINMIC1_DIG           7

#define SIZECOLORPALET       10
#define NUMEFFECTS            4
#define NUM_LEDS_SPEAKER     16
#define NUM_LEDS_BASE         8

#define TIMESTEP_MUSIC 10             // Frequency Filters Param
#define TIMESTEP_LOOP 100             // Led Selection Transition
#define TIMESTEP_COLOR 500            // Color Transition
#define TIMESTEP_EFFECT 20000         // LED Effect Transition
#define TIMELED_OFF 200               // Silence Time to turn off LEDs

#define SOUND_BASS_THRESHOLD 570      // Max 600

// Global Variables ------------------------------------------------

// Timers
elapsedMillis timerSilence;
elapsedMillis timerLoop;
elapsedMillis timerColor;
elapsedMillis timerEffect;

// LED Strip
CRGB ledsSpeaker[NUM_LEDS_SPEAKER];
CRGB ledsBase[NUM_LEDS_BASE];
const CRGB colorPalet[] = {0xF7F7F7, // White Smoke
                           0xBA0034, // Crimson Glory
                           0xF70035, // Carmine Red
                           0xFA07F2,
                           0xDD49B8, // Pink (PANTONE)
                           0xF7F7F7, // White Smoke
                           0x00BAAD, // Amazonite
                           0x00F7E6, // Tuorquoise Blue
                           0xFA07F2,
                           0xDD49B8
                          };// Pink (PANTONE)

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
void (*effectVectorSpeaker[NUMEFFECTS])(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color);
void (*effectVectorBase[NUMEFFECTS])(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color);


// Main functions --------------------------------------------------
void setup() {
  // Setup Pins
  pinMode(PINMIC1_ALG, INPUT);
  pinMode(PINMIC2_ALG, INPUT);
  pinMode(PINMIC1_DIG, INPUT);

  // Start LED Strip
  FastLED.addLeds<WS2811, PINLEDS_SPEAKERS, BRG>(ledsSpeaker, NUM_LEDS_SPEAKER);
  FastLED.addLeds<WS2811, PINLIDS_BASE, BRG>(ledsBase, NUM_LEDS_SPEAKER);

  // Reset all index
  indexEffect = 0;
  indexColor = 0;
  i = 0;

  // Register effects used
  effectVectorBase[0] = LightEffect::LightCollision;
  effectVectorBase[1] = LightEffect::LightFill;
  effectVectorBase[2] = LightEffect::BlockMove;
  effectVectorBase[3] = LightEffect::LightSideFill;
  effectVectorSpeaker[0] = LightEffect::BlockMove;
  effectVectorSpeaker[1] = LightEffect::LightCollision;
  effectVectorSpeaker[2] = LightEffect::BlockX;
  effectVectorSpeaker[3] = LightEffect::BlockRotate;
  
  // Reset Timers
  timerSilence = 0;
  timerLoop = 0;
  timerColor = 0;
  timerEffect = 0;
}

void loop() {

  // Read Mic and set treshold triggers
  int musicInput = analogRead(PINMIC1_ALG);   // Raw Input
  isBeat = digitalRead(PINMIC1_DIG);          // Potentiometer Threshold
  isBass = musicInput > SOUND_BASS_THRESHOLD; // Hardcode Threshold

  // Select Color Timer
  if ( timerColor > TIMESTEP_COLOR)
  {
    timerColor = 0; // Reset Timer
    indexColor = (indexColor < 9) ? indexColor + 1 : 0;
  }

  // Select Effect Timer
  if ( timerEffect > TIMESTEP_EFFECT)
  {
    timerEffect = 0; // Reset Timer
    indexEffect = (indexEffect < 3) ? indexEffect + 1 : 1;
  }

  // Light Effects Timer
  if ( timerLoop > timeStepLedTransition ) // Control Frequency time
  {
    timerLoop = 0; // Reset Timer

    // Alternate from 0 to NUM_LEDS_SPEAKER
    if ( i >= NUM_LEDS_SPEAKER ) inc = -1;
    if ( i <= 0) inc = 1;
    i += inc;
  }

  // Change Effects on Beat
  if (isBeat)
  {
    // Effect Transition Selected by beat frequency
    if (timerSilence > 10)
    {
      timeStepLedTransition = 10*timerSilence;
    }
    else if (timerSilence > 5)
    {
      timeStepLedTransition = 500; // Speed Up transition
    }
    timerSilence = 0;

    // Light Effect
    effectVectorSpeaker[indexEffect](ledsSpeaker,NUM_LEDS_SPEAKER,i,colorPalet[indexColor]);
    effectVectorBase[indexEffect](ledsBase,NUM_LEDS_BASE,i,colorPalet[indexColor]);
    FastLED.show();
  }
  else if (timerSilence > TIMELED_OFF)
  {
    soundSilenceRef = musicInput;
    fill_solid(ledsSpeaker, NUM_LEDS_SPEAKER, 0x000000);
    fill_solid(ledsBase, NUM_LEDS_BASE, 0x000000);
    FastLED.show();
  }
}
