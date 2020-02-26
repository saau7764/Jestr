/*
 * classifier.c
 *
 *  Created on: Jan 21, 2020
 *      Author: samaustin
 */
#include "classifier.h"

void FindGestureFeatures(Gesture_Features_t *gestureFeatures, uint16_t sampleCount)
{
    int i, gIndex, j;
    uint16_t ps_threshold = PS_THRESHOLD;
    uint8_t psState[3] = {0, 0, 0};
    uint16_t ps[3];

    /* Clear Previous Peak values and information */
    for(gIndex = 0; gIndex<GESTURES_SIZE; gIndex++)
    {
        gestureFeatures[gIndex].peakTime[0] = 0;
        gestureFeatures[gIndex].peakTime[1] = 0;
        gestureFeatures[gIndex].peakTime[2] = 0;

        gestureFeatures[gIndex].peakEntryTime[0] = 0;
        gestureFeatures[gIndex].peakEntryTime[1] = 0;
        gestureFeatures[gIndex].peakEntryTime[2] = 0;

        gestureFeatures[gIndex].peakExitTime[0] = 0;
        gestureFeatures[gIndex].peakExitTime[1] = 0;
        gestureFeatures[gIndex].peakExitTime[2] = 0;

        gestureFeatures[gIndex].peakAmplitude[0] = 0;
        gestureFeatures[gIndex].peakAmplitude[1] = 0;
        gestureFeatures[gIndex].peakAmplitude[2] = 0;
    }

    for(gIndex = 0; gIndex<GESTURES_SIZE; gIndex++)
    {
        for(i = 0; i<sampleCount; i++)
        {
            ps[0] = gestureFeatures[gIndex].ps1Counts[i];
            ps[1] = gestureFeatures[gIndex].ps2Counts[i];
            ps[2] = gestureFeatures[gIndex].ps3Counts[i];

            for(j = 0; j<3; j++)
            {
                if(ps[j] >= ps_threshold)
                {
                    if(psState[j] == 0)
                    {
                        gestureFeatures[gIndex].peakEntryTime[j] = i;
                        gestureFeatures[gIndex].peakTime[j] = i;
                        gestureFeatures[gIndex].peakAmplitude[j] = ps[j];
                        psState[j] = 1;
                    }
                    else if(gestureFeatures[gIndex].peakAmplitude[j] < ps[j])
                    {
                        gestureFeatures[gIndex].peakTime[j] = i;
                        gestureFeatures[gIndex].peakAmplitude[j] = ps[j];
                    }
                }
                else if(psState[j] == 1)
                {
                    gestureFeatures[gIndex].peakExitTime[j] = i;
                    psState[j] = 0;
                }
            }
        }
        psState[0] = 0;
        psState[1] = 0;
        psState[2] = 0;
    }
}

void Add_PIR_Data_To_Gesture_Features(Gesture_Features_t *gestureFeatures, Si114x_Sample_t samples, uint16_t index)
{
    gestureFeatures->ps1Counts[index] = samples.ps1;
    gestureFeatures->ps2Counts[index] = samples.ps2;
    gestureFeatures->ps3Counts[index] = samples.ps3;
//    gestureFeatures->xCoord[index] = samples.xCoord;
//    gestureFeatures->yCoord[index] = samples.yCoord;
}

gesture_t DetermineGesture(Gesture_Features_t *gestureFeatures, uint16_t sampleCount)
{
    double dv[12];
    int j;

    FindGestureFeatures(gestureFeatures, sampleCount);

    for(j = 0; j<3; j++)
    {
        dv[j + 0] = gestureFeatures->peakTime[j];
        dv[j + 3] = gestureFeatures->peakEntryTime[j];
        dv[j + 6] = gestureFeatures->peakExitTime[j];
        dv[j + 9] = gestureFeatures->peakAmplitude[j];
    }
    return classifyX(dv);


}

void ClearGestureFeatureValues(Gesture_Features_t *gestureFeatures)
{
    int i;

    for(i = 0; i<BUFFER_SIZE; i++)
    {
        gestureFeatures->ps1Counts[i] = 0;
        gestureFeatures->ps2Counts[i] = 0;
        gestureFeatures->ps3Counts[i] = 0;
//        gestureFeatures->xCoord[i] = 0;
//        gestureFeatures->yCoord[i] = 0;
    }

    for(i = 0; i<3; i++)
    {
        gestureFeatures->peakAmplitude[i] = 0;
        gestureFeatures->peakEntryTime[i] = 0;
        gestureFeatures->peakExitTime[i] = 0;
        gestureFeatures->peakTime[i] = 0;
    }
}


void add_pir_data_to_buffers(CircBuf_t *buf1, CircBuf_t *buf2, CircBuf_t *buf3, Si114x_Sample_t samples)
{
    /* Fill PS data into circular ring buffers */
    add_item_to_buffer(buf1,samples.ps1);
    add_item_to_buffer(buf2,samples.ps2);
    add_item_to_buffer(buf3,samples.ps3);
}

void remove_pir_data_from_buffers(CircBuf_t *buf1, CircBuf_t *buf2, CircBuf_t *buf3, Gesture_Features_t *gestureFeatures, uint16_t length)
{
    int i;
    for(i = 0; i<length; i++)
    {
        gestureFeatures->ps1Counts[i] = remove_item_from_buffer(buf1);
        gestureFeatures->ps2Counts[i] = remove_item_from_buffer(buf2);
        gestureFeatures->ps3Counts[i] = remove_item_from_buffer(buf3);
    }
}









//                if(samples.valid & ALL_DATA)
//                {
//                    gesture = ProcessSi114xSamples(&samples);
//                }


//                if(samples.valid & ALL_DATA)
//                {
//                    //gesture = ProcessSi114xSamples(&samples);
//                    add_item_to_buffer(dataBuffer1,samples.ps1);
//                    add_item_to_buffer(dataBuffer2,samples.ps2);
//                    add_item_to_buffer(dataBuffer3,samples.ps3);
//                    add_item_to_buffer(dataBuffer4,samples.xCoord);
//                    add_item_to_buffer(dataBuffer5,samples.yCoord);
//
//
//                    if(gestureTimer == BUFFER_SIZE - 5)
//                    {
//                        for(i = 0; i<BUFFER_SIZE; i++)
//                        {
//                            gestureFeatures[gestureIndex].ps1Counts[i] = remove_item_from_buffer(dataBuffer1);
//                            gestureFeatures[gestureIndex].ps2Counts[i] = remove_item_from_buffer(dataBuffer2);
//                            gestureFeatures[gestureIndex].ps3Counts[i] = remove_item_from_buffer(dataBuffer3);
//                            gestureFeatures[gestureIndex].xCoord[i] = remove_item_from_buffer(dataBuffer4);
//                            gestureFeatures[gestureIndex].yCoord[i] = remove_item_from_buffer(dataBuffer5);
//                        }
//                        gestureIndex++;
//                        gestureTimer = 0;
//
//                        if(gestureIndex == GESTURES_SIZE)
//                        {
//                            FindGestureFeatures(gestureFeatures);
//                            for(j = 0; j<3; j++)
//                            {
//                                dv[j + 0] = gestureFeatures[gestureIndex - 1].peakTime[j];
//                                dv[j + 3] = gestureFeatures[gestureIndex - 1].peakEntryTime[j];
//                                dv[j + 6] = gestureFeatures[gestureIndex - 1].peakExitTime[j];
//                                dv[j + 9] = gestureFeatures[gestureIndex - 1].peakAmplitude[j];
//                            }
//                            gesture = classifyX(dv);
//                            gestureIndex = 0;
//                        }
//                    }
//                    else if(gestureTimer)
//                    {
//                        gestureTimer++;
//                    }
//                    else if(((samples.ps1 > ps_threshold) || (samples.ps2 > ps_threshold) || (samples.ps3 > ps_threshold)) && (gestureTimer == 0))
//                    {
//                        gestureTimer++;
//                    }
//                }
