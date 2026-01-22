#include <FastLED.h>

// Define the number of LEDs
#define NUM_LEDS 60

// Define the data pin
#define DATA_PIN 2

// Create the LED array
CRGB leds[NUM_LEDS];

// Define the brightness (0-255)
uint8_t brightness = 50;

// Number of LEDs in the sun group
int groupSize = 5;

// How long to wait between each move (in milliseconds)
int moveDelay = 50;

void setup() {
  // Initialize the FastLED library
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // Set the brightness
  FastLED.setBrightness(brightness);
}

void loop() {
  // Emulate moving sun
  emulateMovingSun();
}

// Function to emulate moving sun effect
void emulateMovingSun() {

    // Move the sun group across the LED strip
    for (int i = -groupSize; i < NUM_LEDS; i++) {
      // Clear all LEDs
      fill_solid(leds, NUM_LEDS, CRGB::Black);

      // Light up the sun group
      for (int j = 0; j < groupSize; j++) {
        if (i + j >= 0 && i + j < NUM_LEDS) {
          // Calculate the blend amount based on the position in the strip
          uint8_t blendAmount = map(i + j, 0, NUM_LEDS - 1, 0, 255);
          leds[i + j] = blend(CRGB::Yellow, CRGB::Red, blendAmount);
        }
      }

      FastLED.show();
      delay(moveDelay);
    }
}
