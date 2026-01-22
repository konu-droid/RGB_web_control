#include <FastLED.h>

// Define the number of LEDs
#define NUM_LEDS 60

// Define the data pin
#define DATA_PIN 2

// Create the LED array
CRGB leds[NUM_LEDS];

// Variables to store the current color and the next color
CRGB currentColor = CRGB::Red;
CRGB nextColor = CRGB::Green;

// How many steps to transition
int transitionSteps = 255;

// How long to wait between steps (in milliseconds)
int transitionDelay = 10;

// Define the brightness (0-255)
uint8_t brightness = 10;

void setup() {
  // Initialize the FastLED library
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // Set the brightness
  FastLED.setBrightness(brightness);
}

void loop() {
  // Gradually blend from the current color to the next color
  for (int i = 0; i < transitionSteps; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
      leds[j] = blend(currentColor, nextColor, i);
    }
    FastLED.show();
    delay(transitionDelay);
  }

  // Move to the next color
  currentColor = nextColor;

  // Determine the next color to transition to
  if (currentColor == CRGB::Red) {
    nextColor = CRGB::Green;
  } else if (currentColor == CRGB::Green) {
    nextColor = CRGB::Blue;
  } else {
    nextColor = CRGB::Red;
  }
}
