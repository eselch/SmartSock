#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "FSR.h"

// NeoPixel Configuration
#define LED_PIN 13
#define NUM_LEDS 8

// FSR Configuration - add/remove ports here to change active FSRs
uint8_t FSR_ports[] = {FSR_PORT_1, FSR_PORT_2, FSR_PORT_3, FSR_PORT_4, FSR_PORT_5, FSR_PORT_6, FSR_PORT_7};
const uint8_t numFSRs = sizeof(FSR_ports) / sizeof(FSR_ports[0]);
const uint8_t STATUS_LED = 0;  // Status LED is at position 0

uint16_t FSR_values[MAX_FSR_CHANNELS];
uint8_t brightness[MAX_FSR_CHANNELS];

// Create NeoPixel strip object
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  initFsrAdc();
  
  // Calibrate all FSRs at startup (sets baseline to 0 for unconnected sensors)
  calibrateFsrs(FSR_ports, numFSRs);
  
  strip.begin();
  strip.show();
  
  // Set status LED to green to indicate ready
  strip.setPixelColor(STATUS_LED, strip.Color(0, 12, 0));
  strip.show();
}

void loop() {
  // Read FSRs and map to brightness
  for (uint8_t i = 0; i < numFSRs; i++) {
    FSR_values[i] = readFsr(FSR_ports[i], i);
    
    // Map FSR value to brightness (0-25), constrain to valid range
    // Higher pressure = brighter LED (capped at 25 for battery savings)
    brightness[i] = constrain(map(FSR_values[i], 0, 600, 0, 12), 0, 12);
    
    // Print FSR value
    Serial.print(FSR_values[i]);
    Serial.print(",");
    
    // Set corresponding LED brightness (white color)
    // FSR 1 → LED 1, FSR 2 → LED 2, etc. (1-based mapping)
    strip.setPixelColor(i + 1, strip.Color(brightness[i], brightness[i], brightness[i]));
  }
  
  Serial.println();  // New line after all values
  
  // Keep status LED green (dimmed)
  strip.setPixelColor(STATUS_LED, strip.Color(0, 12, 0));
  strip.show();
  
  delay(10);  // Small delay for stability
}