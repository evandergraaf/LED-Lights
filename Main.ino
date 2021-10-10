#include <FastLED.h>
//added in for neopixel
#include <Adafruit_NeoPixel.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

/**
* CSCI 342 - Dr. Branton - Stretch Assignment
* Controlling LED strip by Arduino and joystick.
*
* Original Code by Mark Kriegsman 
*
* Edited by Ean Vandergraaf 11/24/2020
**/

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

//FastLED
#define LED    6
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    100
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

//Added in for Neopixel
Adafruit_NeoPixel pixels(NUM_LEDS, LED, NEO_GRB + NEO_KHZ800);

// Arduino pin numbers for joystick
// Added in for joystick
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = A0; // analog pin connected to X output
const int Y_pin = A1; // analog pin connected to Y output


void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LED,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
// All patterns from original code, except for colorGlitter
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, colorGlitter};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

// Added colors to loop through in colorGlitter()
CRGB colors[] = {
  CRGB(255, 0, 0), //red
  CRGB(255, 125, 0), //orange
  CRGB(255, 255, 0), //yellow
  CRGB(125, 255, 0), //yellowGreen
  CRGB(0, 255, 0), //green
  CRGB(0, 255, 125), //turquoise
  CRGB(0, 255, 255), //cyan
  CRGB(0, 125, 255), //ocean
  CRGB(0, 0, 255), //blue
  CRGB(125, 0, 255), //violet
  CRGB(255, 0, 255), //magenta
  CRGB(255, 0, 255) //raspberry
};
int gCurrentColorNum = 0; //the index for which color is currently used

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

  /**Added in to allow joystick to interact with the strip**/
  //if user moves joystick all the way to the right
  //move to the next pattern
  if(analogRead(X_pin) == 1023) { 
    //function that changes to the next pattern
    nextPattern(); 
    //delays so that the patterns don't change too fast
    delay(1000);
  };
  //if user moves joystick all the way to the left
  //move to the previous pattern
  if(analogRead(X_pin) == 0){
    //function that changes to the previous pattern
    prevPattern();
    delay(1000);
  }

  //if on the colorGlitter pattern and the user moves the joy stick up
  if(gCurrentPatternNumber == 6 && analogRead(Y_pin) == 1023){
    //change the background color to the next color
    nextColor();
    //delay so that patterns don't change too fast
    delay(1000);
  }
  //if on the colorGlitter pattern and the user moves the joy stick down
  if(gCurrentPatternNumber == 6 && analogRead(Y_pin) == 0){
    //change the background color to the previous color
    prevColor();
    delay(1000);
  }
}

//All part of the original code
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void prevPattern(){
  if(gCurrentPatternNumber == 0){
    gCurrentPatternNumber = ARRAY_SIZE(gPatterns) - 1;
  }
  else{
    gCurrentPatternNumber = (gCurrentPatternNumber - 1) % ARRAY_SIZE(gPatterns);
  }
}

void nextColor(){
  //
  gCurrentColorNum = (gCurrentColorNum + 1) % ARRAY_SIZE(colors);
}

void prevColor(){
  if(gCurrentColorNum == 0){
    gCurrentColorNum = ARRAY_SIZE(colors) - 1;
  }
  else{
    gCurrentColorNum = (gCurrentColorNum - 1) % ARRAY_SIZE(colors);
  }
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
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

//Added in pattern that will change color whenever joystick moves up or down
void colorGlitter(){
    fill_solid(leds, NUM_LEDS, colors[gCurrentColorNum]);
    addGlitter(80);
}