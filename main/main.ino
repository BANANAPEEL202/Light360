#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    15
#define BRIGHTNESS  128
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 75
CRGBPalette16 currentPalette;
TBlendType    currentBlending;


int mode; 
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data
boolean newData = false;

void setup() { 
	Serial.begin(115200); 
	Serial.setTimeout(1); 

  delay( 1000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
    
  currentPalette = PartyColors_p;
  currentBlending = LINEARBLEND;

} 
void loop() { 
	  recvWithStartEndMarkers();
    if (newData) {

        if (String(receivedChars).indexOf("colors") != -1) {
            parseColors(String(receivedChars));
            
        }
        newData = false;
    }
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    FillLEDsFromPaletteColors( startIndex);
    
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
} 

void AddColorToPalette(uint8_t startIndex, uint8_t endIndex, uint8_t r,uint8_t g, uint8_t b)
{
    for( int i = startIndex; i < endIndex; ++i) {
        currentPalette[i] = CRGB(g, r, b);
    }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 1;
    }
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}


// Function to parse the colors message
void parseColors(String msg) {
    int startIndex = msg.indexOf("colors:") + 7; // Skip "colors:" part
    String colorsPart = msg.substring(startIndex);

    // Split the string based on the pipe symbol '|'
    int firstDelimiter = colorsPart.indexOf('|');
    String numColorsStr = colorsPart.substring(0, firstDelimiter); // Extract number of colors
    int numColors = numColorsStr.toInt();
    
    int increment = int(16/numColors);

    // Process each color
    int colorStart = firstDelimiter + 1;
    for (int i = 0; i < numColors; i++) {
        int nextDelimiter = colorsPart.indexOf('|', colorStart);

        // If there are no more delimiters, this is the last color
        if (nextDelimiter == -1) {
            nextDelimiter = colorsPart.length();
        }

        // Extract the color in "R, G, B" format
        String color = colorsPart.substring(colorStart, nextDelimiter);
        int comma1 = color.indexOf(',');
        int comma2 = color.lastIndexOf(',');

        // Extract RGB values
        int r = color.substring(0, comma1).toInt();
        int g = color.substring(comma1 + 1, comma2).toInt();
        int b = color.substring(comma2 + 1).toInt();

        // Print the extracted RGB values
        AddColorToPalette(i*increment, (i+1)*increment, r, g, b);

        // Move to the next color
        colorStart = nextDelimiter + 1;
    }
}

