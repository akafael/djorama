/**
 * LED Strip Effects
 * 
 * @author Rafael Lima
 * @version 0.2
 */

#include <FastLED.h>

struct LightEffect
{
    /**
     * Effect: Turn On all LEDs
     */
    void static TurnOn(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
    {
        fill_solid(ledStrip,stripSize,color);
    }

    /**
     * Effect: move LED on position
     */
    void static LightMoving(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
    {
        fill_solid(ledStrip,stripSize,0x000000);
        ledStrip[k] = color;
    }

    /**
     * Effect: Turn On LEDS in sequence
     */
    void static LightFill(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
    {
        fill_solid(ledStrip,stripSize,0x000000);
        fill_solid(ledStrip,k,color);
    }

    /**
     * Effect: move LEDs position start to side and 
     *         turn on everything when they reach the center
     */
    void static LightCollision(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
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
    void static LightDots(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
    {
        fill_solid(ledStrip,stripSize,0x000000);
        ledStrip[k] = color;
        ledStrip[stripSize-k] = color;
    }

    /**
     * Effect: move LEDs position starting from sides and 
     *         turn on everything when they reach the center
     */
    void static LightSideFill(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
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
    void static BlockMove(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
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
    void static Alternate(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
    {
        int hitOdds = k%(stripSize/2);
        fill_solid(ledStrip,stripSize,0x000000);
        fill_solid(ledStrip+2*k+hitOdds,1,color);
    }

    /**
     * Effect: Light a segment
     */
    void static BlockRotate(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
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
    void static BlockX(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
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
    void static Blink(CRGB* ledStrip, unsigned int stripSize, unsigned int k, CRGB color)
    {
        if( k%2 )
        {
            fill_solid(ledStrip,stripSize,color);
        }
        else
        {
            fill_solid(ledStrip,stripSize,0);
        }   
    }
};


