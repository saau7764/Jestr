/*
 * si114x.c
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */
#include "si114x.h"

extern volatile uint8_t Si114x_INT_Pending;
extern volatile uint8_t ResumeSampling;
extern volatile uint8_t PauseSampling;
extern volatile Si114x_State_t pirStatus;

int16_t Init_Si114x(uint8_t start)
{
    int retval = 0;
    uint16_t taskList = PS1_TASK |          // To measure PS1
                       PS2_TASK |           // To measure PS2
                       PS3_TASK |           // To measure PS3
                       ALS_VIS_TASK |       // To measure VIS
                       ALS_IR_TASK;         // To measure IR
    uint16_t adcGain = ADC_NORM;            // PS_ADC_GAIN to default div by 1
    uint16_t adcMisc = PS_MEAS_MODE;        // PS_ADC_MISC to default signal range
    uint16_t adcMux[3] = {0x03,0x03,0x03};  // PS1_ADCMUX, PS2_ADCMUX, PS3_ADCMUX to default large photodiode

    retval += ResetSi114x(); // reset si114x

    /* set IR LED current for si114x */
    retval += WriteSi114xRegister(REG_PS_LED21,0xff);
    retval += WriteSi114xRegister(REG_PS_LED3 ,0xf);

    /* Initialize CHLIST Parameter from caller to enable measurement */
    retval += SetSi114xParameter(PARAM_CH_LIST, taskList);

    /* Setting adc parameters on si114x */
    retval += SetSi114xParameter(PARAM_PS_ADC_GAIN, adcGain);
    retval += SetSi114xParameter(PARAM_PS_ADC_MISC, adcMisc);
    retval += SetSi114xParameter(PARAM_PS1_ADC_MUX, adcMux[0]);
    retval += SetSi114xParameter(PARAM_PS2_ADC_MUX, adcMux[1]);
    retval += SetSi114xParameter(PARAM_PS3_ADC_MUX, adcMux[2]);

    /* Configure the ALS IR channel for the same settings as PS */
    retval += SetSi114xParameter(PARAM_ALSIR_ADC_GAIN, adcGain);
    retval += SetSi114xParameter(PARAM_ALSIR_ADC_MISC, adcMisc & ALS_IR_ADC_MISC_MASK);
    retval += SetSi114xParameter(PARAM_IR_ADC_MUX, adcMux[0]);

    // If nothing went wrong after all of this time, the value
    // returned will be 0. Otherwise, it will be some negative
    // number
    return (int16_t)retval;
}

int16_t ResetSi114x()
{
    int retval = 0;
    // Do not access the Si114x earlier than 25 ms from power-up.
    delay_10ms();
    delay_10ms();
    delay_10ms();

    // Note that the Si114xReset() actually performs the following functions:
    //     1. Pauses all prior measurements
    retval += WriteSi114xRegister(REG_MEAS_RATE, 0x00);
    retval += WriteSi114xRegister(REG_ALS_RATE,  0x00);
    retval += Si114xPauseAll();
    //     2. Clear  i2c registers that need to be cleared
    retval += WriteSi114xRegister(REG_MEAS_RATE,  0x00);
    retval += WriteSi114xRegister(REG_IRQ_ENABLE, 0x00);
    retval += WriteSi114xRegister(REG_IRQ_MODE1,  0x00);
    retval += WriteSi114xRegister(REG_IRQ_MODE2,  0x00);
    retval += WriteSi114xRegister(REG_INT_CFG,    0x00);
    //     3. Clears irq status to make sure INT* is negated
    //        Perform reset command
    retval += WriteSi114xRegister(REG_IRQ_STATUS, 0xFF);
    retval += WriteSi114xRegister(REG_COMMAND, RESET);
    //     4. Delays 10 ms
    delay_10ms();
    //     5. Sends HW Key
    retval += WriteSi114xRegister(REG_HW_KEY, HW_KEY_VAL0);

    return (int16_t)retval;
}

int16_t SendSi114xCommand(uint8_t command)
{
    int16_t  response;
    int   retval;
    uint8_t  count = 0;

    // Get the response register contents
    response = ReadSi114xRegister(REG_RESPONSE);
    if(response < 0) return response;

    // Double-check the response register is consistent
    while(count < LOOP_TIMEOUT_MS)
    {
      if((retval = waitUntilSleep()) != 0) return (int16_t)retval;

      if(command == 0) break; // Skip if the command is NOP

      retval = ReadSi114xRegister(REG_RESPONSE);
      if(retval == response) break;
      else if(retval < 0) return (int16_t)retval;
      else response = retval;
      count++;
    }

    // Send the Command
    if((retval = WriteSi114xRegister(REG_COMMAND, command)) != 0)
      return (int16_t)retval;

    count = 0;
    // Expect a change in the response register
    while(count < LOOP_TIMEOUT_MS)
    {
      if(command == 0) break; // Skip if the command is NOP

      retval = ReadSi114xRegister(REG_RESPONSE);
      if(retval != response) break;
      else if(retval < 0) return (int16_t)retval;
      count++;
      delay_1ms();
    }
    return 0;
}

int16_t Si114xPauseAll()
{
    int countA, countB;
    int  retval;


    //  After a RESET, if the Si114x receives a command (including NOP) before the
    //  Si114x has gone to sleep, the chip hangs. This first while loop avoids
    //  this.  The reading of the REG_CHIPSTAT does not disturb the internal MCU.
    //

    retval = 0; //initialize data so that we guarantee to enter the loop
    while(retval != 0x01)
    {
        retval = ReadSi114xRegister(REG_CHIP_STAT);
        if (retval != 0x01) delay_1ms();
    }

    countA = 0;
    while(countA < LOOP_TIMEOUT_MS)
    {
        countB = 0;
        // Keep sending nops until the response is zero
        while(countB < LOOP_TIMEOUT_MS)
        {
            retval = ReadSi114xRegister(REG_RESPONSE);
            if( retval == 0 ) break;
            else
            {
                // Send the NOP Command to clear any error...we cannot use Si114xNop()
                // because it first checks if REG_RESPONSE < 0 and if so it does not
                // perform the cmd. Since we have a saturation REG_RESPONSE will be <0
                WriteSi114xRegister(REG_COMMAND, 0x00);
            }
            countB++;
            delay_1ms();
        }

        // Pause the device
        SendSi114xCommand(PSALS_PAUSE);

        countB = 0;
        // Wait for response
        while(countB < LOOP_TIMEOUT_MS)
        {
            retval = ReadSi114xRegister(REG_RESPONSE);
            if( retval != 0 ) break;
            countB++;
            delay_1ms();
        }

        // When the PsAlsPause() response is good, we expect it to be a '1'.
        retval = ReadSi114xRegister(REG_RESPONSE);
        if( retval == 1 ) break;  // otherwise, start over.
        countA++;
    }
    return 0;
}

int16_t SetSi114xParameter(uint8_t reg, uint8_t value)
{
    int retval;
    uint8_t buffer[2];
    int16_t response_stored;
    int16_t response;

    // Return if i2c error occured
    if((retval = waitUntilSleep())!=0) return (int16_t)retval;

    // Get response from si114x
    response_stored = ReadSi114xRegister(REG_RESPONSE);

    // IDK what this does ???????
    buffer[0]= value;
    buffer[1]= 0xA0 + (reg & 0x1F);

    // idk why we are doing this ????
    retval = WriteSi114xRegister(REG_PARAM_WR, buffer[0]);
    if(retval != 0) return (int16_t)retval;
    retval = WriteSi114xRegister(REG_COMMAND, buffer[1]);
    if(retval != 0) return (int16_t)retval;


    // Wait for command to finish
    response = ReadSi114xRegister(REG_RESPONSE);
    while(response == response_stored)
    {
        response = ReadSi114xRegister(REG_RESPONSE);
        if (response == response_stored) delay_1ms();
    }

    if(retval < 0) return (int16_t)retval;
    else return 0;
}


int16_t GetSi114xParameter(uint8_t reg)
{
    int retval;
    uint8_t cmd = 0x80 + (reg & 0x1f);

    retval = SendSi114xCommand(cmd);
    if(retval != 0) return (int16_t)retval;

    retval = ReadSi114xRegister(REG_PARAM_RD);
    return (int16_t)retval;;
}

static int16_t waitUntilSleep()
{
    // Waits until the Si114x is sleeping before proceeding

    int retval = -1;
    int count = 0;
    // This loops until the Si114x is known to be in its sleep state or if an i2c error occurs
    while(count < LOOP_TIMEOUT_MS)
    {
      retval = ReadSi114xRegister(REG_CHIP_STAT);
      if(retval == 1) break;
      if(retval <  0) return (int16_t)retval;
      count++;
      delay_1ms();
    }
    return 0;
}

int16_t ReadSi114xRegister(uint8_t reg)
{
    int retval;
    uint8_t data;

    // Ask I2C peripheral to read byte at reg and place into data
    retval = I2C_Read_Byte(SI114x_ID, reg, &data);

    // If retval is negitive then error happened, else return data
    if(retval < 0){
        return (int16_t)retval;
    }
    else return data;

}

int16_t WriteSi114xRegister(uint8_t reg, uint8_t data)
{
    int retval;
    // Ask I2C peripheral to write byte of data at reg
    retval = I2C_Write_Byte(SI114x_ID, reg, data);
    return (int16_t)retval;
}

int16_t WriteBlockSi114xRegister(uint8_t reg, uint8_t length, uint8_t *data)
{
    unsigned int counter;
    int retval;

    for(counter = 0; counter<length; counter++)
    {
        retval = WriteSi114xRegister(reg+counter, data[counter]);
    }
    return (int16_t)retval;
}

int16_t ReadBlockSi114xRegister(uint8_t reg, uint8_t length, uint8_t *data)
{
    unsigned int counter;
    int retval;

    for(counter = 0; counter<length; counter++)
    {
        retval = ReadSi114xRegister(reg+counter);
        if(retval < 0) return (int16_t)retval;
        else data[counter] = retval;
    }
    return 0;
}

void ClearSi114xINT(uint8_t clearBits)
{
    while(WriteSi114xRegister(REG_IRQ_STATUS, clearBits))
    {
        delay_1ms();
    }
}

void PausePIRSampling()
{
    SysTick->CTRL &= ~(NVIC_ST_CTRL_ENABLE);
    PauseSampling = 0; // Clear flag
    SetLed(0,0,0,0);   // Clear LEDs
    pirStatus = paused;
}

void ResumePIRSampling()
{
    ResumeSampling = 0; // Clear flag
    SetLed(0,0,0,0);    // Clear LEDs
    pirStatus = idle;
    SysTick->CTRL |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}

