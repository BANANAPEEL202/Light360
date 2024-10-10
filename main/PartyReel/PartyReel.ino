/// @file    DemoReel100.ino
/// @brief   FastLED "100 lines of code" demo reel, showing off some effects
/// @example DemoReel100.ino

#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014


#define DATA_PIN    12
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {centerOutwards, pulse };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 15 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 30, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 120;
  CRGBPalette16 palette = 	HeatColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// Center-outwards animation
void centerOutwards() {
  int middle = NUM_LEDS / 2 + 10;
  CRGBPalette16 palette = 	RainbowColors_p;
  uint8_t colorIndex = random8();  // Get a random index for color

  // Get the random color from the palette
  CRGB color = ColorFromPalette(palette, colorIndex);

  // Clear all LEDs
  FastLED.clear();

  // Light up the middle LEDs first
  if (NUM_LEDS % 2 == 0) {
    leds[middle - 1] = color;
    leds[middle] = color;
  } else {
    leds[middle] = color;
  }
  FastLED.show();

  

  // Light up outward from the center
  int speed = 3;
  for (int i = 1; i <= middle; i += speed) {
    fadeToBlackBy(leds, NUM_LEDS, 10);
    for (int j = 0; j < speed; j++) {
      if (middle - i - j >= 0) {
        leds[middle - i - j] = color;  // Light to the left of center
      }
      if (middle + i + j < NUM_LEDS) {
        leds[middle + i + j] = color;  // Light to the right of center
      }
    }
    FastLED.show();
  }
}

void pulse() {
  // Use a random color from the PartyColors palette
  static CRGBPalette16 palette = RainbowColors_p;

  // Create a random color from the palette
  uint8_t colorIndex = random8();
  CRGB color = ColorFromPalette(palette, colorIndex);

  // Define pulsing parameters
  static uint8_t brightness = 0;
  static int8_t direction = 1;  // 1 for increasing, -1 for decreasing

  // Check if we are fading in or out
  

  // Fill the LED strip with the color and apply brightness scaling
  fill_solid(leds, NUM_LEDS, color.nscale8(255));

  // Update the LED strip
  FastLED.show();
  for (int i = 0; i < 50; i ++ ){
     fadeToBlackBy( leds, NUM_LEDS, 10);
     FastLED.show();
  }

  // Optional: Add a small delay for smooth pulsing
  delay(30); // Adjust delay for speed of pulsing
}