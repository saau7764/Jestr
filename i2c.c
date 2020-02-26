/*
 * i2c.c
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#include "i2c.h"

/** A transfer state information for an ongoing master mode transfer. */
extern volatile I2C_Transfer_t i2cTransfer;


void Init_I2C()
{
    /* I2C0 configuration for communication with si114x */
    I2C0->MCR  |= I2C_MCR_MFE;          // enable master
    I2C0->PC &=  ~I2C_PC_HS;            // Set to Standard/Fast Mode
    I2C0->MTPR =  0xe;                  // TPR = (120MHz /(2*10*400Kbps))-1 = 0xe

    // Wait 30 mSec Before running any i2c communication
    delay_10ms();
    delay_10ms();
    delay_10ms();

}

I2C_TransferReturn_t I2C_Write_Byte(uint8_t addr, uint8_t reg, uint8_t data)
{
    //uint32_t count = 0;

    i2cTransfer.state = i2cMasterTransmit;
    i2cTransfer.result = i2cTransferInProgress;
    i2cTransfer.addr = addr;
    i2cTransfer.reg = reg;
    i2cTransfer.flag = I2C_FLAG_WRITE;
    i2cTransfer.index = 0;
    i2cTransfer.length = 1;
    i2cTransfer.data = &data;

    /* Clear all pending interrupts prior to starting a transfer. */
    I2C0->MICR |= CLEARALL_I2C_INTERRPUTS;

    /* Enable I2C0 interrupt in NVIC module */
    if(addr == SI114x_ID)
    {
        I2C0->MIMR |= I2C_MIMR_STOPIM       // STOP Detection Interrupt
                   |  I2C_MIMR_NACKIM       // Address/Data NACK Interrupt
                   |  I2C_MIMR_IM;          // Master transaction complete Interrupt
        NVIC_EnableIRQ(I2C0_IRQn);
    }
    else return i2cTransferUsageFault;


    /***************************************************/
    /* Start I2C process to send Byte */
    /***************************************************/

    I2C0->MSA = (addr << 1);                    // Set I2C Slave Address/next master operation is transmit
    I2C0->MDR = reg;                            // Place register address in master data register
    I2C0->MCS = I2C_MCS_RUN | I2C_MCS_START;    // Enable master and set start condition

    while(!(i2cTransfer.state == i2cMasterIdle));

    NVIC_DisableIRQ(I2C0_IRQn);
    I2C0->MICR |= CLEARALL_I2C_INTERRPUTS;

    return i2cTransfer.result;
}

I2C_TransferReturn_t I2C_Read_Byte(uint8_t addr, uint8_t reg, uint8_t *data)
{
    //uint32_t count = 0;
    i2cTransfer.state = i2cMasterTransmit;
    i2cTransfer.result = i2cTransferInProgress;
    i2cTransfer.addr = addr;
    i2cTransfer.reg = reg;
    i2cTransfer.flag = I2C_FLAG_WRITE_READ;
    i2cTransfer.index = 0;
    i2cTransfer.length = 1;
    i2cTransfer.data = data;

    /* Clear all pending interrupts prior to starting a transfer. */
    I2C0->MICR = CLEARALL_I2C_INTERRPUTS;

    /* Enable I2C0 interrupt in NVIC module */
    if(addr == SI114x_ID)
    {
        I2C0->MIMR |= I2C_MIMR_STOPIM       // STOP Detection Interrupt
                   |  I2C_MIMR_NACKIM       // Address/Data NACK Interrupt
                   |  I2C_MIMR_IM;          // Master interrupt
        NVIC_EnableIRQ(I2C0_IRQn);
    }
    else return i2cTransferUsageFault;


    /***************************************************/
    /* Send S+ADDR(W). */
    /***************************************************/

    I2C0->MSA = (SI114x_ID << 1);               // Set I2C Slave Address/next master operation is transmit
    I2C0->MDR = reg;                            // Place slave id in master data register
    I2C0->MCS = I2C_MCS_RUN | I2C_MCS_START;    // Enable master and set start condition

    while(!(i2cTransfer.state == i2cMasterIdle));

    NVIC_DisableIRQ(I2C0_IRQn);
    I2C0->MICR = CLEARALL_I2C_INTERRPUTS;

    return i2cTransfer.result;

}

void I2C0_IRQHandler(void)
{
    uint32_t pending, status;

    pending = I2C0->MMIS;   // Get pending interrupts
    I2C0->MICR = pending;   // Clear interrupts
    //I2C0->MIMR = 0;         // disable interrupts

    /* Process next receive or transmit */
    if((pending & I2C_MIMR_IM) == I2C_MIMR_IM)
    {
        /* Send Repeated start condition for reading data*/
        if((i2cTransfer.state == i2cMasterTransmit) && (i2cTransfer.flag == I2C_FLAG_WRITE_READ))
        {
            i2cTransfer.state = i2cMasterReceive;
            I2C0->MSA = (i2cTransfer.addr << 1) | BIT0;
            if(i2cTransfer.length == 1)
            {
                I2C0->MCS = I2C_MCS_RUN | I2C_MCS_START | I2C_MCS_STOP;
            }
            else if(i2cTransfer.length > 1)
            {
                I2C0->MCS = I2C_MCS_RUN | I2C_MCS_START | I2C_MCS_ACK;
            }
            else
            {
                i2cTransfer.result = i2cTransferSwFault;
            }
        }
        /* Send next byte of data*/
        else if((i2cTransfer.state == i2cMasterTransmit) && (i2cTransfer.index < i2cTransfer.length -1))
        {
            I2C0->MDR = i2cTransfer.data[i2cTransfer.index++];
            I2C0->MCS = I2C_MCS_RUN;
        }
        /* Send last byte of data*/
        else if((i2cTransfer.state == i2cMasterTransmit) && (i2cTransfer.index == i2cTransfer.length -1))
        {
            I2C0->MDR = i2cTransfer.data[i2cTransfer.index++];
            I2C0->MCS = I2C_MCS_RUN | I2C_MCS_STOP;
        }
        /* Receive next byte of data */
        else if((i2cTransfer.state == i2cMasterReceive) && (i2cTransfer.index < i2cTransfer.length -2))
        {
            i2cTransfer.data[i2cTransfer.index++] = I2C0->MDR;
            I2C0->MCS = I2C_MCS_RUN | I2C_MCS_ACK; // idk if need the ack part
        }
        /* Receive second to last byte of data */
        else if((i2cTransfer.state == i2cMasterReceive) && (i2cTransfer.index == i2cTransfer.length -2))
        {
            i2cTransfer.data[i2cTransfer.index++] = I2C0->MDR;
            I2C0->MCS = I2C_MCS_RUN | I2C_MCS_STOP;
        }
    }

    /* Process i2c after stop condition has been sent */
    if((pending & I2C_MIMR_STOPIM) == I2C_MIMR_STOPIM)
    {
        if(i2cTransfer.state == i2cMasterTransmit)
        {
            i2cTransfer.state = i2cMasterIdle;
            i2cTransfer.result = i2cTransferDone;
        }
        else if(i2cTransfer.state == i2cMasterReceive)
        {
            i2cTransfer.data[i2cTransfer.index++] = I2C0->MDR;
            i2cTransfer.state = i2cMasterIdle;
            i2cTransfer.result = i2cTransferDone;
        }
        else
        {
            i2cTransfer.state = i2cMasterIdle;
        }
    }

    /* Process errors */
    if((pending & I2C_MIMR_NACKIM) == I2C_MIMR_NACKIM)
    {
        status = I2C0->MCS;
        if(!(status & I2C_MCS_BUSY))
        {
            if(status & (I2C_MCS_ERROR | I2C_MCS_ARBLST))
            {
                /* Check for Nack after sending slave address */
                if((status & I2C_MCS_ADRACK) == I2C_MCS_ADRACK)
                {
                    i2cTransfer.state = i2cMasterErr;
                    i2cTransfer.result = i2cTransferAddrNack;
                }
                /* Check for Nack after sending data */
                if((status & I2C_MCS_DATACK) == I2C_MCS_DATACK)
                {
                    i2cTransfer.state = i2cMasterErr;
                    i2cTransfer.result = i2cTransferDataNack;
                }
                /* Check if arbitration was lost */
                if((status & I2C_MCS_ARBLST) == I2C_MCS_ARBLST)
                {
                    i2cTransfer.state = i2cMasterErr;
                    i2cTransfer.result = i2cTransferArbLost;
                }
                if(!(pending & I2C_MIMR_STOPIM))
                {
                    I2C0->MCS = I2C_MCS_STOP;
                }
            }
        }
    }

    I2C0->MICR = pending;   // Clear interrupts
}
