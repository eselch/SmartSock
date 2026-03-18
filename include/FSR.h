#ifndef FSR_h
#define FSR_h

#include <stdint.h>

/*
Call initFsrAdc() from your init function

Call readFsr(channel, index), where:
  - channel is the analog pin (e.g., FSR_PORT_1)
  - index is the FSR index (0-6) for baseline storage

This will return the reading with baseline subtracted
*/
#define MAX_FSR_CHANNELS 7

#define FSR_PORT_1 A0
#define FSR_PORT_2 A1
#define FSR_PORT_3 A2
#define FSR_PORT_4 A3
#define FSR_PORT_5 A4
#define FSR_PORT_6 A5
#define FSR_PORT_7 A11  // Shared with D12

void initFsrAdc(void);
void calibrateFsrs(uint8_t* ports, uint8_t numPorts);
uint16_t readFsr(uint8_t channel, uint8_t index);

#endif
