// Main.ino: written to be able to loop through different patterns 
// Got patterns from (https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/)
// Fall 2020 - Ean Vandergraaf

//www.elegoo.com
//2016.12.08
#include <Adafruit_NeoPixel.h>
#include "SR04.h"

//the LEDs
#define LED 6
#define NUMPIXELS 100
 
//for joystick (https://create.arduino.cc/projecthub/Raushancpr/control-rgb-led-with-joystick-68f601)
// Arduino pin numbers
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = A0; // analog pin connected to X output
const int Y_pin = A1; // analog pin connected to Y output
int buttonState = 0;

//for changing the colors based off joystick
const int redX = 512;
const int redY = 1023;
const int greenX = 1023;
const int greenY = 0;
const int blueX = 0;
const int blueY = 0;

//whenever the joystick is moved to left or right the pattern will change
String state = "follow";
Adafruit_NeoPixel pixels(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);
void setup() {
   pinMode(SW_pin, INPUT);
  
   Serial.begin(9600);
   Serial.println("Starting up...");
   
   pixels.begin();
   pixels.clear();
}

void loop() {
   pixels.clear();
  
  if(state == "follow"){
     int light = analogRead(X_pin) / 10;
     pixels.setPixelColor(light, 0, 0, 255);
  }
  if(state == "controlAll"){
    //change the colors based on the joystick
    int xAxis = analogRead(X_pin);
    int yAxis = analogRead(Y_pin);
    int distanceRed = sqrt(pow(abs(redX - xAxis), 2) +  pow(abs(redY - yAxis), 2));
    int distanceGreen = sqrt(pow(abs(greenX - xAxis), 2) +  pow(abs(greenY - yAxis), 2));
    int distanceBlue = sqrt(pow(abs(blueX - xAxis), 2) +  pow(abs(blueY - yAxis), 2));
  
    int brightRed = 255 - constrain(map(distanceRed, 0, 1023, 0, 255), 0, 255);
    int brightGreen = 255 - constrain(map(distanceGreen, 0, 1023, 0, 255), 0, 255);
    int brightBlue = 255 - constrain(map(distanceBlue, 0, 1023, 0, 255), 0, 255);
    //color all the lights
    setAll(brightRed, brightBlue, brightGreen);
  }

  buttonState = digitalRead(SW_pin);

  if(buttonState == HIGH){
    rainbowCycle(10);
  }

  pixels.show();
  delay(10);

}

void setAll(int red, int blue, int green){
  for(int i = 0; i < NUMPIXELS; i++){
      pixels.setPixelColor(i, red, blue, green);
    }
}

// From (https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/)
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUMPIXELS; i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / NUMPIXELS) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
