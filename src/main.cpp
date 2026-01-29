#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "FSR.h"

// NeoPixel Configuration
#define LED_PIN 9
#define NUM_LEDS 8

// FSR Configuration - add/remove ports here to change active FSRs
uint8_t FSR_ports[] = {FSR_PORT_1};
const uint8_t numFSRs = sizeof(FSR_ports) / sizeof(FSR_ports[0]);
const uint8_t STATUS_LED = numFSRs;  // Status LED is right after the FSR LEDs

uint16_t FSR_values[MAX_FSR_CHANNELS];
uint8_t brightness[MAX_FSR_CHANNELS];

// Create NeoPixel strip object
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  initFsrAdc();
  strip.begin();
  strip.show();
  
  // Set status LED to green to indicate ready
  strip.setPixelColor(STATUS_LED, strip.Color(0, 255, 0));
  strip.show();
}

void loop() {
  // Read FSRs and map to brightness
  for (uint8_t i = 0; i < numFSRs; i++) {
    FSR_values[i] = readFsr(FSR_ports[i], i);
    
    // Map FSR value to brightness (0-255), constrain to valid range
    // Higher pressure = brighter LED
    brightness[i] = constrain(map(FSR_values[i], 0, 900, 0, 255), 0, 255);
    
    // Print FSR value
    Serial.print(FSR_values[i]);
    Serial.print(",");
    
    // Set corresponding LED brightness (white color)
    strip.setPixelColor(i, strip.Color(brightness[i], brightness[i], brightness[i]));
  }
  
  Serial.println();  // New line after all values
  
  // Keep status LED green
  strip.setPixelColor(STATUS_LED, strip.Color(0, 255, 0));
  strip.show();
  
  delay(10);  // Small delay for stability
}