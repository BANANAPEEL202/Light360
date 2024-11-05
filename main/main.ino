#include <WiFi.h>             // or <ESP8266WiFi.h> for ESP8266
#include <ESPAsyncWebServer.h> // Library to manage the asynchronous server
#include <FastLED.h>

FASTLED_USING_NAMESPACE

// Replace these with your Wi-Fi credentials
const char* ssid = "GTother";
const char* password = "GeorgeP@1927";

AsyncWebServer server(80);    // Set up a server on port 80

String mode = "party";


#define DATA_PIN    12
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    300
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  // Start Serial
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Route to return status
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", "{\"status\":\"" + mode + "\"}");
  });

  // Route to control device
  server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("action")) {
      String action = request->getParam("action")->value();
      action.toUpperCase();
      if (action == "OFF") {
        mode = "OFF";
        request->send(200, "application/json", "{\"message\":\"Mode: OFF\"}");
      } else if (action == "PACIFICA") {
        mode = "PACIFICA";
        request->send(200, "application/json", "{\"message\":\"Mode: PACIFICA\"}");
      } else if (action == "PARTY"){
        mode = "PARTY";
        request->send(200, "application/json", "{\"message\":\"Mode: PARTY\"}");
      }
      else if (action == "WHITE"){
        mode = "WHITE";
        request->send(200, "application/json", "{\"message\":\"Mode: WHITE\"}");
      }
    } else {
      request->send(400, "application/json", "{\"error\":\"No action specified\"}");
    }
    Serial.println("Changed mode to: " + mode);
  });

  // Start the server
  server.begin();


  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {centerOutwards, pulse };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop() {
  if (mode == "OFF"){
    fill_solid(leds, NUM_LEDS, CRGB::Black); // Set all LEDs to black
    FastLED.show();                          // Update the LED strip
  }
  else if (mode == "PARTY"){
    // Call the current pattern function once, updating the 'leds' array
    gPatterns[gCurrentPatternNumber]();
    turnOffFirstLEDS()
    // send the 'leds' array out to the actual LED strip
    FastLED.show();  
    // insert a delay to keep the framerate modest
    FastLED.delay(1000/FRAMES_PER_SECOND); 

    // do some periodic updates
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    EVERY_N_SECONDS( 15 ) { nextPattern(); } // change patterns periodically
  }
  else if (mode == "PACIFICA") {
    EVERY_N_MILLISECONDS( 20) {
      pacifica_loop();
      turnOffFirstLEDS()
      FastLED.show();
    }
  }
  else if (mode == "WHITE"){
    uint8_t whiteBrightness = 200; // Set the desired brightness level (0 to 255)
    fill_solid(leds, NUM_LEDS, CRGB::White.nscale8(whiteBrightness)); // Scale the white color
    turnOffFirstLEDS()
    FastLED.show();                          // Update the LED strip
    delay(10); // Short delay to allow for other processes
  }
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

void turnOffFirstLEDS() {
  for (int i = 0; i < 10 && i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black; // Set first 10 LEDs to off
  }
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


//////////////////////////////////////////////////////////////////////////
//
// The code for this animation is more complicated than other examples, and 
// while it is "ready to run", and documented in general, it is probably not 
// the best starting point for learning.  Nevertheless, it does illustrate some
// useful techniques.
//
//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.  
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then 
// another filter is applied that adds "whitecaps" of brightness where the 
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent 
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//
CRGBPalette16 pacifica_palette_1 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
    { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 0x000117, 
      0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
    { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 0x000C33, 
      0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 0x2060FF };


void pacifica_loop()
{
  // Increment the four "color index start" counters, one for each wave layer.
  // Each is incremented at a different speed, and the speeds vary over time.
  static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
  static uint32_t sLastms = 0;
  uint32_t ms = GET_MILLIS();
  uint32_t deltams = ms - sLastms;
  sLastms = ms;
  uint16_t speedfactor1 = beatsin16(3, 179, 269);
  uint16_t speedfactor2 = beatsin16(4, 179, 269);
  uint32_t deltams1 = (deltams * speedfactor1) / 256;
  uint32_t deltams2 = (deltams * speedfactor2) / 256;
  uint32_t deltams21 = (deltams1 + deltams2) / 2;
  sCIStart1 += (deltams1 * beatsin88(1011,10,13));
  sCIStart2 -= (deltams21 * beatsin88(777,8,11));
  sCIStart3 -= (deltams1 * beatsin88(501,5,7));
  sCIStart4 -= (deltams2 * beatsin88(257,4,6));

  // Clear out the LED array to a dim background blue-green
  fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));

  // Render each of four layers, with different scales and speeds, that vary over time
  pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
  pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4,  6 * 256,  9 * 256), beatsin8( 17, 40,  80), beat16( 401) );
  pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
  pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));

  // Add brighter 'whitecaps' where the waves lines up more
  pacifica_add_whitecaps();

  // Deepen the blues and greens a bit
  pacifica_deepen_colors();
}

// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff)
{
  uint16_t ci = cistart;
  uint16_t waveangle = ioff;
  uint16_t wavescale_half = (wavescale / 2) + 20;
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    waveangle += 250;
    uint16_t s16 = sin16( waveangle ) + 32768;
    uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
    ci += cs;
    uint16_t sindex16 = sin16( ci) + 32768;
    uint8_t sindex8 = scale16( sindex16, 240);
    CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
    leds[i] += c;
  }
}

// Add extra 'white' to areas where the four layers of light have lined up brightly
void pacifica_add_whitecaps()
{
  uint8_t basethreshold = beatsin8( 9, 55, 65);
  uint8_t wave = beat8( 7 );
  
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
    wave += 7;
    uint8_t l = leds[i].getAverageLight();
    if( l > threshold) {
      uint8_t overage = l - threshold;
      uint8_t overage2 = qadd8( overage, overage);
      leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
    }
  }
}

// Deepen the blues and greens
void pacifica_deepen_colors()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    leds[i].blue = scale8( leds[i].blue,  145); 
    leds[i].green= scale8( leds[i].green, 200); 
    leds[i] |= CRGB( 2, 5, 7);
  }
}
