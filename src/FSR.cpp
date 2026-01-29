#include <Arduino.h>
#include "FSR.h"
#include <stdint.h>

// Static baseline storage for each channel - persists between calls
static uint16_t fsrBaseline[MAX_FSR_CHANNELS] = {0};
static bool isCalibrated[MAX_FSR_CHANNELS] = {false};

void initFsrAdc(void) {
  // Use DEFAULT (VCC = 5V) reference for ATmega32U4 (Pro Micro)
  analogReference(DEFAULT);
  // Reset calibration on init
  for (uint8_t i = 0; i < MAX_FSR_CHANNELS; i++) {
    isCalibrated[i] = false;
    fsrBaseline[i] = 0;
  }
}

uint16_t readFsr(uint8_t channel, uint8_t index) {
  // First call for this channel: calibrate by averaging readings for 100 milliseconds
  if (index < MAX_FSR_CHANNELS && !isCalibrated[index]) {
    uint32_t sum = 0;
    uint16_t numSamples = 0;
    unsigned long startTime = millis();
    
    while (millis() - startTime < 100) {
      sum += analogRead(channel);
      numSamples++;
    }
    
    fsrBaseline[index] = sum / numSamples;
    isCalibrated[index] = true;
  }
  
  uint16_t reading = analogRead(channel);
  uint16_t baseline = (index < MAX_FSR_CHANNELS) ? fsrBaseline[index] : 0;

  // Handle both FSR types: ones that increase and ones that decrease with pressure
  if (reading >= baseline) {
    return reading - baseline;
  } else {
    return baseline - reading;
  }
}




