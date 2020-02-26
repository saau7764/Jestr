/*
 * si114x_algorithm.h
 *
 *  Created on: Dec 8, 2019
 *      Author: samaustin
 */

#ifndef SI114X_ALGORITHM_H_
#define SI114X_ALGORITHM_H_

#include "si114x.h"
#include "si114x_defs.h"
#include "gpio.h"
#include "defines.h"

typedef enum {
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  TAP,
  ERROR
} gesture_t;

typedef enum {
    WaitingForGesture,
    GestureInProgress,
    GestureComplete
} Gesture_Status_t;


void GetPIRData(uint32_t timeStamp, Si114x_Sample_t *samples);
gesture_t ProcessSi114xSamples(Si114x_Sample_t *sample);
uint8_t isIRStable(Si114x_Sample_t *sample);
void IRCompensation(uint8_t channel, Si114x_Sample_t *sample, uint8_t ircorrection[]);
void FindHandPosition(Si114x_Sample_t *samples);
uint16_t PSCount2Distance(uint16_t counts);
uint8_t CheckPIRThresholds(Si114x_Sample_t samples);
void ShowGestureType(gesture_t gesture);

#endif /* SI114X_ALGORITHM_H_ */


