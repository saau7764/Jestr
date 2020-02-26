/*
 * si114x_algorithm.c
 *
 *  Created on: Dec 8, 2019
 *      Author: samaustin
 */

#include "si114x_algorithm.h"


void GetPIRData(uint32_t timeStamp, Si114x_Sample_t *samples)
{
    int16_t data0,data1;
    /* IR correction coeff. for indoors enviorments */
    uint8_t ircorrection[3] = {17, 35, 29};

    samples->timeStamp = timeStamp;
    samples->valid = 0;
    samples->xCoord = 0;
    samples->yCoord = 0;

    /*read VIS data from si114x */
    data0 = ReadSi114xRegister(REG_ALS_VIS_DATA0);
    data1 = ReadSi114xRegister(REG_ALS_VIS_DATA1);
    if((data0 > 0) && (data1 > 0)) samples->valid |= BIT4;
    samples->vis = (uint8_t)data0 | ((uint8_t)data1 << 8);

    /*read IR data from si114x */
    data0 = ReadSi114xRegister(REG_ALS_IR_DATA0);
    data1 = ReadSi114xRegister(REG_ALS_IR_DATA1);
    if((data0 > 0) && (data1 > 0)) samples->valid |= BIT3;
    samples->ir = (uint8_t)data0 | ((uint8_t)data1 << 8);

    /*read PS1 data from si114x */
    data0 = ReadSi114xRegister(REG_PS1_DATA0);
    data1 = ReadSi114xRegister(REG_PS1_DATA1);
    if((data0 > 0) && (data1 > 0)) samples->valid |= BIT0;
    samples->ps1 = (uint8_t)data0 | ((uint8_t)data1 << 8);

    /*read PS2 data from si114x */
    data0 = ReadSi114xRegister(REG_PS2_DATA0);
    data1 = ReadSi114xRegister(REG_PS2_DATA1);
    if((data0 > 0) && (data1 > 0)) samples->valid |= BIT1;
    samples->ps2 = (uint8_t)data0 | ((uint8_t)data1 << 8);

    /*read PS3 data from si114x */
    data0 = ReadSi114xRegister(REG_PS3_DATA0);
    data1 = ReadSi114xRegister(REG_PS3_DATA1);
    if((data0 > 0) && (data1 > 0)) samples->valid |= BIT2;
    samples->ps3 = (uint8_t)data0 | ((uint8_t)data1 << 8);

    /* IR Ambient Compensation for all PS channels */
    IRCompensation(0, samples, ircorrection);
    IRCompensation(1, samples, ircorrection);
    IRCompensation(2, samples, ircorrection);

}

gesture_t ProcessSi114xSamples(Si114x_Sample_t *samples)
{
    static uint32_t ps_entry_time[3] = { 0, 0, 0 };
    static uint32_t ps_exit_time[3]  = { 0, 0, 0 };
    static uint8_t ps_state[3]       = { 0, 0, 0 };
    uint16_t ps[3];
    uint16_t ps_threshold = PS_THRESHOLD;
    uint32_t diff_x;
    uint32_t diff_y1;
    uint32_t diff_y2;
    uint32_t ps_time[3];
    uint32_t ps_avg;
    gesture_t  ret = NONE;  /*gesture result return value */
    uint32_t i;

    /*save new samples into ps array */
    ps[0] = samples->ps1;
    ps[1] = samples->ps2;
    ps[2] = samples->ps3;

    /* Check state of all three measurements */
    for (i = 0; i < 3; i++){
        /* If measurement higher than the ps_threshold value, */
        /*   record the time of entry and change the state to look for the exit time */
        if (ps[i] >= ps_threshold){
            if (ps_state[i] == 0) {
                ps_state[i] = 1;
                ps_entry_time[i] = samples->timeStamp;
            }
        }
        else{
            if (ps_state[i] == 1){
                ps_state[i] = 0;
                ps_exit_time[i] = samples->timeStamp;
            }
        }
    }

    /* If there is no object in front of the board, look at history to see if a gesture occured */
    if ((ps[0] < ps_threshold) && (ps[1] < ps_threshold) && (ps[2] < ps_threshold))
    {
        /* If the ps_max values are high enough and there exit entry and exit times, */
        /*   then begin processing gestures */
        if ((ps_entry_time[0] != 0) && (ps_entry_time[1] != 0) && (ps_entry_time[2] != 0)
                && (ps_exit_time[0] != 0) && (ps_exit_time[1] != 0) && (ps_exit_time[2] != 0))
        {
            /* Make sure no timestamps overflowed, indicated possibility if any of them are close to overflowing */
            /* If any of them are close to overflowing, overflow them all so they all have the same reference */
            if ((ps_exit_time[0] > 0xFC000000L) || (ps_exit_time[1] > 0xFC000000L) || (ps_exit_time[2] > 0xFC000000L)
                    || (ps_entry_time[0] > 0xFC000000L) || (ps_entry_time[1] > 0xFC000000L) || (ps_entry_time[2] > 0xFC000000L))
            {
                ps_exit_time[0] += 0x1FFFFFFFL;
                ps_exit_time[1] += 0x1FFFFFFFL;
                ps_exit_time[2] += 0x1FFFFFFFL;

                ps_entry_time[0] += 0x1FFFFFFFL;
                ps_entry_time[1] += 0x1FFFFFFFL;
                ps_entry_time[2] += 0x1FFFFFFFL;
            }
            /* Calculate the midpoint (between entry and exit times) of each waveform */
            /*  the order of these midpoints helps determine the gesture */
            ps_time[0] = (ps_exit_time[0] - ps_entry_time[0]) / 2;
            ps_time[0] = ps_time[0] + ps_entry_time[0];

            ps_time[1] = (ps_exit_time[1] - ps_entry_time[1]) / 2;
            ps_time[1] = ps_time[1] + ps_entry_time[1];

            ps_time[2] = (ps_exit_time[2] - ps_entry_time[2]) / 2;
            ps_time[2] = ps_time[2] + ps_entry_time[2];

            /* The diff_x and diff_y values help determine a gesture by comparing the */
            /*  LED measurements that are on a single axis */
            if (ps_time[1] > ps_time[2])
            {
                diff_x = ps_time[1] - ps_time[2];
            }
            else
            {
                diff_x = ps_time[2] - ps_time[1];
            }
            if ( ps_time[0] > ps_time[1] )
            {
                diff_y1 = ps_time[0] - ps_time[1];
            }
            else
            {
                diff_y1 = ps_time[1] - ps_time[0];
            }
            if ( ps_time[0] > ps_time[2] ) {
                diff_y2 = ps_time[0] - ps_time[2];
            }
            else
            {
                diff_y2 = ps_time[2] - ps_time[0];
            }

            /* Take the average of all three midpoints to make a comparison point for each midpoint */
            ps_avg = (uint32_t) ps_time[0] + (uint32_t) ps_time[1] + (uint32_t) ps_time[2];
            ps_avg = ps_avg / 3;

            if ((ps_exit_time[0] - ps_entry_time[0]) > 10 || (ps_exit_time[1] - ps_entry_time[1]) > 10 || (ps_exit_time[2] - ps_entry_time[2]) > 10)
            {
                if (((ps_time[1] < ps_time[0]) &&  (diff_y1 > diff_x)) || ( (ps_time[1] < ps_time[2]) && (diff_y1 > diff_y2)))
                { /* An up gesture occured if the bottom LED had its midpoint first */
                    ret = UP;
                }
                /* A down gesture occured if the bottom LED had its midpoint last */
                else if (((ps_time[0] < ps_time[1]) &&  (diff_y1 > diff_x)) || ( (ps_time[2] < ps_time[1]) && (diff_y1 > diff_y2)))
                {
                    ret = DOWN;
                }
                /* A left gesture occured if the left LED had its midpoint last */
                else if (((ps_time[2] < ps_time[0]) && (diff_y2 > diff_y1)) || ((ps_time[2] < ps_time[1]) && (diff_x > diff_y1)))
                {
                    ret = LEFT;
                }
                /* A right gesture occured if the right LED had midpoint later than the right LED */
                else if (((ps_time[0] < ps_time[2]) && (diff_y2 > diff_y1)) || ((ps_time[1] < ps_time[2]) && (diff_x > diff_y1)))
                {
                    ret = RIGHT;
                }
            }
        }
        for (i = 0; i < 3; i++)
        {
            ps_exit_time[i]  = 0;
            ps_entry_time[i] = 0;
        }
    }
    return ret;
}

uint16_t PSCount2Distance(uint16_t counts)
{
    uint16_t distanceLSB;
    uint16_t distanceMSB;
    uint32_t distance;
    uint8_t indexLinear;
    uint8_t maxIndex = 7;
    uint16_t offset[] = {53, 73, 96, 125, 170, 237, 421};
    uint16_t slope[] = {53, 93, 186, 420, 1142, 3275, 15054};
    uint16_t piecewise_brackets[] = {32768, 16384, 8192, 4096, 2048, 1024, 512};

    // Perform piecewise linear approximation
    for (indexLinear = 0; indexLinear < maxIndex; indexLinear++)
    {
        if (counts > piecewise_brackets[indexLinear])
        {
            distance = (uint32_t)counts * (uint32_t)slope[indexLinear];
            distanceMSB = (distance & 0xffff0000) >> 16;
            distanceLSB = offset[indexLinear] - distanceMSB;
            break;
        }
    }

    // Set to a max value if above a certain level.
    if (distanceLSB > 250 || indexLinear == maxIndex)
    {
         distanceLSB = 250;
    }

    return (distanceLSB);
}

void FindHandPosition(Si114x_Sample_t *samples)
{
    uint16_t r1, r2, r3;
    uint16_t dx = 363;
    uint16_t dy = 383;
    int x, y;

    r1 = PSCount2Distance(samples->ps1);
    r2 = PSCount2Distance(samples->ps2);
    r3 = PSCount2Distance(samples->ps3);

    // calculate x and y
    // x = (r1^2 - r3^2 + d^2) / (2 * d) + offset
    // y = (r1^2 - r2^2 + d^2) / (2 * d) + offset
    // Coordinates (0,0) are located at the bottom left
    // corner of the board
    x = ((r1*r1) - (r3*r3) + (dx*dx))/(2*dx);
    y = ((r2*r2) - (r1*r1) + (dy*dy))/(2*dy);

    if(x < 0) x = 0;
    if(y < 0) y = 0;

    samples->xCoord = x;
    samples->yCoord = y;
}


uint8_t CheckPIRThresholds(Si114x_Sample_t samples)
{
    uint8_t retval = 0;
    if(samples.valid & ALL_DATA)
    {
        if(samples.ps1 >= PS_THRESHOLD) retval |= BIT0;
        if(samples.ps2 >= PS_THRESHOLD) retval |= BIT1;
        if(samples.ps3 >= PS_THRESHOLD) retval |= BIT2;
    }
    return retval;
}

uint8_t isIRStable(Si114x_Sample_t *samples)
{
    static uint32_t IRlast = 0;        // Contains the ALS IR reading from the previous measurement cycle

    // Initialize IRlast to the ALS IR reading if it does not contain
    // information from a previous measurement cycle
    if( IRlast == 0) IRlast = samples->ir;

    // If ALS IR is saturated, return false immediately
    else if( samples->ir > 48000 ) return 0;

    if( samples->ir < (IRlast + IRlast/2) && samples->ir > (IRlast - IRlast/2))
    {
        // If IR is within 3 dB, declare it stable
        // and perform some averaging
        IRlast = (samples->ir + (IRlast*3)) / 4;
        return 1;
    }
    else
    {
        // Otherwise, return false and update
        // new IRlast value for next time
        IRlast = samples->ir;
        return 0;
    }
}

void IRCompensation(uint8_t channel, Si114x_Sample_t *sample, uint8_t ircorrection[])
{
    uint32_t Correction;             // IR Correction factor

    // Calculate correction factor based on the ALS IR + PS reading
    // subtracting out the 256 count offset contained in both measurements
    // Full equation comes out to:
    // Correction = ( (ALS_IR - 256) + (PS - 256) ) * IRcorrectionfactor
    //  where IRcorrectionfactor = IRCORRECTION / 10000
    switch(channel)
    {
        case 0:
            Correction = ((uint32_t)sample->ir + (uint32_t)(sample->ps1) - 512) * (uint32_t) ircorrection[channel];
            Correction = Correction/10000;
            sample->ps1 = sample->ps1 + Correction;
            break;
        case 1:
            Correction = ((uint32_t)sample->ir + (uint32_t)(sample->ps2) - 512) * (uint32_t) ircorrection[channel];
            Correction = Correction/10000;
            sample->ps2 = sample->ps2 + Correction;
            break;
        case 2:
            Correction = ((uint32_t)sample->ir + (uint32_t)(sample->ps3) - 512) * (uint32_t) ircorrection[channel];
            Correction = Correction/10000;
            sample->ps3 = sample->ps3 + Correction;
            break;
        default:
            break;
    }
}
void ShowGestureType(gesture_t gesture)
{
    switch(gesture)
    {
    case UP:
        SetLed(0,1,1,1);
        break;
    case DOWN:
        SetLed(1,1,1,0);
        break;
    case LEFT:
        SetLed(1,1,0,0);
        break;
    case RIGHT:
        SetLed(0,0,1,1);
        break;
    case TAP:
        SetLed(0,1,1,0);
        break;
    case ERROR:
        SetLed(0,0,0,0);
    case NONE:
        break;
    default:
        break;
    }
}




