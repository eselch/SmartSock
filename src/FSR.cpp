#include <Arduino.h>
#include "FSR.h"
#include <stdint.h>

// Noise threshold - readings below this are treated as 0
#define FSR_NOISE_THRESHOLD 40

// Static baseline storage for each channel - persists between calls
static uint16_t fsrBaseline[MAX_FSR_CHANNELS] = {0};
static bool isCalibrated[MAX_FSR_CHANNELS] = {false};

void initFsrAdc(void) {
  // Use DEFAULT (VCC = 3.3V) reference for ATmega32U4 (Feather 32u4)
  analogReference(DEFAULT);
  // Reset calibration on init
  for (uint8_t i = 0; i < MAX_FSR_CHANNELS; i++) {
    isCalibrated[i] = false;
    fsrBaseline[i] = 0;
  }
}

void calibrateFsrs(uint8_t* ports, uint8_t numPorts) {
  // Calibrate all FSRs at startup by averaging readings for 100ms each
  for (uint8_t i = 0; i < numPorts && i < MAX_FSR_CHANNELS; i++) {
    uint32_t sum = 0;
    uint16_t numSamples = 0;
    unsigned long startTime = millis();
    
    while (millis() - startTime < 100) {
      sum += analogRead(ports[i]);
      numSamples++;
    }
    
    fsrBaseline[i] = sum / numSamples;
    isCalibrated[i] = true;
  }
}

uint16_t readFsr(uint8_t channel, uint8_t index) {
  // Discard first reading to let ADC settle after channel switch
  analogRead(channel);
  delayMicroseconds(100);
  
  // First call for this channel: calibrate by averaging readings for 250 milliseconds
  if (index < MAX_FSR_CHANNELS && !isCalibrated[index]) {
    uint32_t sum = 0;
    uint16_t numSamples = 0;
    unsigned long startTime = millis();
    
    while (millis() - startTime < 250) {
      sum += analogRead(channel);
      numSamples++;
    }
    
    fsrBaseline[index] = sum / numSamples;
    isCalibrated[index] = true;
  }
  
  uint16_t reading = analogRead(channel);
  uint16_t baseline = (index < MAX_FSR_CHANNELS) ? fsrBaseline[index] : 0;

  // Inverted FSR: high voltage at rest, drops under pressure
  // Return 0 if reading is at or above baseline, otherwise return the difference
  if (reading >= baseline) {
    return 0;
  }
  
  uint16_t pressure = baseline - reading;
  
  // Filter out noise: readings below threshold are treated as 0
  if (pressure < FSR_NOISE_THRESHOLD) {
    return 0;
  }
  return pressure;
}




