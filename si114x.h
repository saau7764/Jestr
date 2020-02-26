/*
 * si114x.h
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#ifndef SI114X_H_
#define SI114X_H_

#include "si114x_defs.h"
#include "i2c.h"
#include "timer.h"
#include "gpio.h"

typedef struct {
    uint16_t ps1;               // PS1
    uint16_t ps2;               // PS2
    uint16_t ps3;               // PS3
    uint16_t vis;               // VIS
    uint16_t ir;                // IR
    uint8_t valid;              // valid status
    uint32_t timeStamp;         // Timestamp to record
    uint16_t xCoord;                 // Estimated x position
    uint16_t yCoord;                 // Estimated y position
} Si114x_Sample_t;

typedef enum {
  idle,
  getSample,
  paused,
  waitForSample
} Si114x_State_t;

int16_t Init_Si114x(uint8_t start);
int16_t ResetSi114x();
int16_t SendSi114xCommand(uint8_t command);
int16_t Si114xPauseAll();
int16_t ReadSi114xRegister(uint8_t reg);
int16_t WriteSi114xRegister(uint8_t reg, uint8_t data);
int16_t SetSi114xParameter(uint8_t reg, uint8_t value);
int16_t GetSi114xParameter(uint8_t reg);
int16_t WriteBlockSi114xRegister(uint8_t reg, uint8_t length, uint8_t *data);
int16_t ReadBlockSi114xRegister(uint8_t reg, uint8_t length, uint8_t *data);
static int16_t waitUntilSleep();
void ClearSi114xINT(uint8_t clearBits);
void PausePIRSampling();
void ResumePIRSampling();

#endif /* SI114X_H_ */
