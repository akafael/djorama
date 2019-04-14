/**
 * Djorama Controler
 * 
 * Features:
 *  - React to Sound Volume
 *  - Music Detection by Silence Time
 *  - Independent timer for changing effect, color and speed
 *  - LED Strip Controled By Music
 *  - DC Motor Controled By Music
 * 
 * @author Rafael Lima
 * @version 0.4 
 */

// Libraries -------------------------------------------------------
#include <elapsedMillis.h>
#include <FastLED.h>
#include <Servo.h>

// Constants -------------------------------------------------------
#define PINLEDBASE         9
#define PINLEDCITY         10
#define PINLEDDJ           11
#define PINMIC1_ALG        A0
#define PINMIC1_DIG        7
#define PINMOTORDC1        12
#define PINMOTORDC2        11 

// LED Strip Position and size
#define NUM_LEDS_BASE      16
#define NUM_LEDS_DJ        11 
#define NUM_LEDS_CITY      5 // 2+2+1 
#define LED_CITYBULD1A     0
#define LED_CITYBULD1B     1
#define LED_CITYBULD2      2
#define LED_CITYBULD3      3

#define SIZECOLORPALET 10
#define NUMEFFECTS 4

#define TIMESTEP_MUSIC 10             // Frequency Filters Param
#define TIMESTEP_LOOP 100             // Led Selection Transition
#define TIMESTEP_COLOR 500            // Color Transition
#define TIMESTEP_EFFECT 20000         // LED Effect Transition
#define TIMESTEP_MOTORDC 10           // Max Aceleration (5/255)/TIMESTEP_MOTORDC
#define TIMESTEP_SERVO 10             // 100Hz
#define TIMELED_OFF 200               // Silence Time to turn off LEDs
#define TIMESHUTDOWN_MOTORDC 5000     // Silence Time to turn down motor

/* 
 * Max time for motor to start to move 
 *  TIMESTEP_MOTORDC*(SPEED_MOTORDC_ZERO - SPEED_MOTORDC_SAFEZERO)
 */
#define SPEED_MOTORDC_SAFEZERO 30     // lowest Tension reach with music
#define SPEED_MOTORDC_ZERO     80     // lowest Tension for start to move

#define SOUND_BASS_THRESHOLD 570      // Max 600

// Global Variables ------------------------------------------------

// Timers
elapsedMillis timerSilence;
elapsedMillis timerLoop;
elapsedMillis timerColor;
elapsedMillis timerEffect;
elapsedMillis timerMotorDC;
elapsedMillis timerServo;

// LED Strip
CRGB ledsBase[NUM_LEDS_BASE];
CRGB ledsCity[NUM_LEDS_CITY];
CRGB ledsDJ[NUM_LEDS_DJ];
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
unsigned int indexLEDBase = 0;
unsigned int indexLEDDJ = 0;
unsigned int indexLEDCity = 0;
int incLEDBase;
int incLEDDJ;
int incLEDCity;
int isBeat;
int isBass;
int isMusicPlaying = 0;

int timeStepLedTransition = TIMESTEP_LOOP;

int soundSilenceRef = 0;

int speedMotorDC = 0;
int stepMotorDC = 0;

Servo servoHead;
int angleHead;
int stepHead;

Servo servoTable;
int angleTable;
int stepTable;

// Function Pointer Vector for easy effect access
void (*effectVector[NUMEFFECTS])(unsigned int k,CRGB color);

// Main functions --------------------------------------------------
void setup() {
  // Setup Pins
  pinMode(PINMIC1_ALG,INPUT);
  pinMode(PINMIC1_DIG,INPUT);
  pinMode(PINMOTORDC1, OUTPUT);
  pinMode(PINMOTORDC2, OUTPUT);

  // Turn Off DC Motor
  analogWrite(PINMOTORDC1,0);
  analogWrite(PINMOTORDC2,0);
  
  // Start LED Strip
  FastLED.addLeds<WS2811, PINLEDBASE, BRG>(ledsBase, NUM_LEDS_BASE);
  FastLED.addLeds<WS2811, PINLEDCITY, BRG>(ledsCity, NUM_LEDS_CITY);
  FastLED.addLeds<WS2811, PINLEDDJ, BRG>(ledsDJ, NUM_LEDS_DJ);

  // Reset all index
  indexEffect = 0;
  indexColor = 0;
  indexLEDBase = 0;

  // Register effects used
  effectVector[0] = effectLightMoving;
  effectVector[1] = effectLightFill;
  effectVector[2] = effectLightDots;
  effectVector[3] = effectLightSideFill;

  servoHead.attach(42);
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

    // Alternate from 0 to NUM_LEDS_BASE
    if( indexLEDBase >= NUM_LEDS_BASE ) incLEDBase = -1;
    if( indexLEDBase <= 0) incLEDBase = 1;
    indexLEDBase += incLEDBase;

    // Alternate from 0 to NUM_LEDS_DJ
    if( indexLEDDJ >= NUM_LEDS_DJ ) incLEDDJ = -1;
    if( indexLEDDJ <= 0) incLEDBase = 1;
    indexLEDDJ += incLEDDJ;
    
    // Alternate from 0 to NUM_LEDS_CITY
    if( indexLEDCity >= NUM_LEDS_CITY ) incLEDCity = -1;
    if( indexLEDCity <= 0) incLEDCity = 1;
    indexLEDCity += incLEDCity;
  }

  // Motor DC Acceleration Control Timer
  if( timerMotorDC > TIMESTEP_MOTORDC)
  {
    speedMotorDC += stepMotorDC;
    analogWrite(PINMOTORDC1,speedMotorDC);
    analogWrite(PINMOTORDC2,0);
  }

  // Servo Motor Acceleration Control Timer
  if( timerServo > TIMESTEP_SERVO)
  {
    // Move Head
    angleHead += stepHead;
    servoHead.write(angleHead);
  }

  // Change Effects on Beat
  if(isBeat)
  {
    // Effect Transition Selected by beat frequency
    if(timerSilence > 10)
    {
      timeStepLedTransition = 100;
    }
    else if(timerSilence > 5)
    {
      timeStepLedTransition = 100/timerSilence; // Speed Up transition
    }
    timerSilence = 0;

    // Light Effect
    effectVector[indexEffect](indexLEDBase,colorPalet[indexColor]);
    effectVector[indexEffect](indexLEDDJ,colorPalet[indexColor]);
    FastLED.show();

    // Speed Up Motor
    stepMotorDC = (speedMotorDC >=255)?0:1;

    // Move Servo Motor
    stepHead = (angleHead >=180)?-1:(angleHead <= 0) ?1:stepHead;
  }
  else if(timerSilence > TIMELED_OFF)
  {   
    soundSilenceRef = musicInput;
    fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    FastLED.show();

    // Slow Down Motor
    if(timerSilence > TIMESHUTDOWN_MOTORDC)
    {
      stepMotorDC = (speedMotorDC <= SPEED_MOTORDC_SAFEZERO)?0:-1;
    }
    else
    {
      speedMotorDC = (speedMotorDC <= 0)?0:-1;
    }

    // Stop Servo Motor
    stepHead = 0;
  }

  effectVector[indexEffect](indexLEDBase,colorPalet[indexColor]);
}

// Auxilar Functions -------------------------------------------------

/**
 * Effect: Turn On all LEDs
 */
void effectTurnOn(unsigned int k,CRGB color)
{
    fill_solid(ledsBase,NUM_LEDS_BASE,color);
    fill_solid(ledsBase,NUM_LEDS_BASE,color);
}

/**
 * Effect: move LED on position
 */
void effectLightMoving(unsigned int k,CRGB color)
{
    fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    ledsBase[k] = color;
    fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    ledsBase[k] = color;
}

/**
 * Effect: Turn On LEDS in sequence
 */
void effectLightFill(unsigned int k,CRGB color)
{
    fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    fill_solid(ledsBase,k,color);
    fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    fill_solid(ledsBase,k,color);
}

/**
 * Effect: move LEDs position start to side and 
 *         turn on everything when they reach the center
 */
void effectLightCollision(unsigned int k,CRGB color)
{
    if( (NUM_LEDS_BASE - k-k) <= 1)
    {
      fill_solid(ledsBase,NUM_LEDS_BASE,color);
    }
    else
    {
      fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
      ledsBase[k] = color;
      ledsBase[NUM_LEDS_BASE-k] = color;
    }
    
    if( (NUM_LEDS_BASE - k-k) <= 1)
    {
      fill_solid(ledsBase,NUM_LEDS_BASE,color);
    }
    else
    {
      fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
      ledsBase[k] = color;
      ledsBase[NUM_LEDS_BASE-k] = color;
    }

}

/**
 * Effect: move LEDs position starting from sides and 
 *         turn on everything when they reach the center
 */
void effectLightDots(unsigned int k,CRGB color)
{
    fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    ledsBase[k] = color;
    ledsBase[NUM_LEDS_BASE-k] = color;

    fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    ledsBase[k] = color;
    ledsBase[NUM_LEDS_BASE-k] = color;
}

/**
 * Effect: move LEDs position starting from sides and 
 *         turn on everything when they reach the center
 */
void effectLightSideFill(unsigned int k,CRGB color)
{
    int endK = (NUM_LEDS_BASE - k);
    if( endK > k) // Center
    {
      fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
      fill_solid(ledsBase,k,color);
      fill_solid(ledsBase+endK,k,color);
      fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    }
    else
    {
      fill_solid(ledsBase,NUM_LEDS_BASE,color);
      fill_solid(ledsBase+endK,k-endK,0x000000);
    }

    endK = (NUM_LEDS_BASE - k);
    if( endK > k) // Center
    {
      fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
      fill_solid(ledsBase,k,color);
      fill_solid(ledsBase+endK,k,color);
      fill_solid(ledsBase,NUM_LEDS_BASE,0x000000);
    }
    else
    {
      fill_solid(ledsBase,NUM_LEDS_BASE,color);
      fill_solid(ledsBase+endK,k-endK,0x000000);
    }
}
