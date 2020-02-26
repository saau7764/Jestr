/*
 * classifier.h
 *
 *  Created on: Jan 21, 2020
 *      Author: samaustin
 */

#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#include "circbuf.h"
#include "defines.h"
#include "si114x_algorithm.h"
#include "si114x.h"
#include "classifyX.h"

typedef struct{
    uint16_t ps1Counts[BUFFER_SIZE];
    uint16_t ps2Counts[BUFFER_SIZE];
    uint16_t ps3Counts[BUFFER_SIZE];
    uint16_t peakTime[3];
    uint16_t peakEntryTime[3];
    uint16_t peakExitTime[3];
    uint16_t peakAmplitude[3];
//    uint16_t xCoord[BUFFER_SIZE];
//    uint16_t yCoord[BUFFER_SIZE];

}Gesture_Features_t;

void FindGestureFeatures(Gesture_Features_t *gestureFeatures, uint16_t sampleCount);
gesture_t DetermineGesture(Gesture_Features_t *gestureFeatures, uint16_t sampleCount);
void Add_PIR_Data_To_Gesture_Features(Gesture_Features_t *gestureFeatures, Si114x_Sample_t samples, uint16_t index);
void ClearGestureFeatureValues(Gesture_Features_t *gestureFeatures);
void remove_pir_data_from_buffers(CircBuf_t *buf1, CircBuf_t *buf2, CircBuf_t *buf3, Gesture_Features_t *gestureFeatures, uint16_t length);
void add_pir_data_to_buffers(CircBuf_t *buf1, CircBuf_t *buf2, CircBuf_t *buf3, Si114x_Sample_t samples);


#endif /* CLASSIFIER_H_ */
