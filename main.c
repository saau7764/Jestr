/*
 * main.c
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "msp.h"
#include "msp_compatibility.h"
#include "core_cm4.h"
#include "gpio.h"
#include "i2c.h"
#include "timer.h"
#include "uart.h"
#include "si114x.h"
#include "circbuf.h"
#include "classifier.h"
#include "defines.h"
#include "si114x_algorithm.h"
#include "si114x_defs.h"
#include "sysinit.h"

volatile uint8_t ResumeSampling = 0;
volatile uint8_t PauseSampling = 0;
volatile Si114x_State_t pirStatus = paused;
volatile Gesture_Status_t gestureStatus = WaitingForGesture;
volatile I2C_Transfer_t i2cTransfer;
volatile uint8_t gestureIndex = 0;

int main(void)
{
    __disable_interrupt();

    /* Declare local variables */
    Si114x_Sample_t samples;
    gesture_t gesture = NONE;
    CircBuf_t *buf1;
    CircBuf_t *buf2;
    CircBuf_t *buf3;
    uint16_t lastGestureTime = 0;
    uint16_t gestureTimer = 0;
    Gesture_Features_t gestureFeatures[GESTURES_SIZE];
    int i;

    /* Clearing Values in gesture feature structure */
    ClearGestureFeatureValues(gestureFeatures);

    /* creating buffers in dynamic memory */
    buf1 = malloc(sizeof(CircBuf_t));
    initialize_buffer(buf1,CIRC_BUFFER_SIZE);
    buf2 = malloc(sizeof(CircBuf_t));
    initialize_buffer(buf2,CIRC_BUFFER_SIZE);
    buf3 = malloc(sizeof(CircBuf_t));
    initialize_buffer(buf3,CIRC_BUFFER_SIZE);

    /* Initialize system and peripherals and enable interrupts*/
    Init_System();
    Init_GPIO();
    Init_Timers();
    Init_I2C();
    Init_UART();
    Init_SysTick();
    __enable_interrupt();

    /* Initialize Si114x*/
    while(Init_Si114x(0) != 0);
    SetLed(1,0,0,0);
    //ResumePIRSampling();

    while(1)
    {
        if(pirStatus == getSample)
        {
            pirStatus = waitForSample;
            if(SendSi114xCommand(PSALS_FORCE) == 0)
            {
                delay_1ms();
                GetPIRData(gestureTimer,&samples);
                FindHandPosition(&samples);

                if(samples.valid & ALL_DATA)
                {
                    // Fill PS data into circular ring buffers
                    add_pir_data_to_buffers(buf1, buf2, buf3, samples);

                    switch(gestureStatus)
                    {
                    case WaitingForGesture:

                        /* Enter when object is in the FOV of PIR sensor */
                        if(CheckPIRThresholds(samples))
                        {
                            gestureStatus = GestureInProgress;    // Update state of system
                            gestureTimer = buf1->num_items; // Update gesture features indexing
                            remove_pir_data_from_buffers(buf1, buf2, buf3, gestureFeatures, gestureTimer);
                        }
                        /* Any other conditions means nothing has happened since the last sample */
                        else
                        {
                            ClearGestureFeatureValues(gestureFeatures);
                            gestureStatus = WaitingForGesture;
                            gestureTimer = 0;
                        }
                        break;

                    case GestureInProgress:

                        /* If gesture duration is over 1 second then gesture is not valid */
                        if(gestureTimer >= BUFFER_SIZE)
                        {
                            ClearGestureFeatureValues(gestureFeatures);
                            gestureStatus = WaitingForGesture;
                            gestureTimer = 0;
                        }
                        /* Gesture considered to be in progress when object is found in FOV */
                        else if(CheckPIRThresholds(samples))
                        {
                            Add_PIR_Data_To_Gesture_Features(gestureFeatures, samples, gestureTimer++);
                            gestureStatus = GestureInProgress;
                        }
                        /* More then 10 samples gathered means gesture is valid */
                        else if(gestureTimer >= 10)
                        {
                            Add_PIR_Data_To_Gesture_Features(gestureFeatures, samples, gestureTimer++);
                            lastGestureTime = gestureTimer + 5; // set last sample time stamp to be 5 samples from here
                            gestureStatus = GestureComplete;

                        }
                        /* Gesture considered to be invalid when only consists of 10 samples */
                        else
                        {
                            ClearGestureFeatureValues(gestureFeatures);
                            gestureStatus = WaitingForGesture;
                            gestureTimer = 0;
                        }
                        break;

                    case GestureComplete:

                        /* Gesture not valid if duration over 1 second */
                        if(gestureTimer >= BUFFER_SIZE)
                        {
                            ClearGestureFeatureValues(gestureFeatures);
                            gestureStatus = WaitingForGesture;
                            gestureTimer = 0;
                        }
                        /* Receive last PIR data point and determine gesture */
                        else if(gestureTimer >= lastGestureTime)
                        {
                            Add_PIR_Data_To_Gesture_Features(gestureFeatures, samples, gestureTimer++);
                            FindGestureFeatures(gestureFeatures, gestureTimer);
                            //UART_SendGestureFeatures(gestureFeatures);
                            //gesture = DetermineGesture(gestureFeatures, gestureTimer);
                            ClearGestureFeatureValues(gestureFeatures);
                            gestureStatus = WaitingForGesture;
                            gestureTimer = 0;
                        }
                        /* Continue grabbing 5 data points after an object has been present */
                        else
                        {
                            Add_PIR_Data_To_Gesture_Features(gestureFeatures, samples, gestureTimer++);
                            gestureStatus = GestureComplete;
                        }
                        break;


                    default:
                        ClearGestureFeatureValues(gestureFeatures);
                        gestureStatus = WaitingForGesture;
                        gestureTimer = 0;
                        break;
                    }
                }
                else gesture = NONE;
                /***************************************************************/
                ShowGestureType(gesture);   /* LED Display of last valid gesture
                                             *
                                             *         D1   D2   D3   D4
                                             * up    = off  on   on   on
                                             * down  = on   on   on   off
                                             * right = off  off  on   on
                                             * left  = on   on   off  off
                                             * tap   = off  on   on   off
                                             *                            */
                /***************************************************************/
            }
            else  SetLed(0,0,0,0);    // Clear LEDs
            pirStatus = idle;
        }

        /* Button Flags that pause or resume streaming PIR data */
        if(ResumeSampling) ResumePIRSampling();
        if(PauseSampling) PausePIRSampling();
    }
}

