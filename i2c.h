/*
 * i2c.h
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#ifndef I2C_H_
#define I2C_H_

#include "msp.h"
#include <stdbool.h>
#include "si114x_defs.h"
#include "timer.h"

#define I2C_MASTER_DISABLE      0b000000
#define CLEARALL_I2C_INTERRPUTS 0xfff
#define I2C_FLAG_WRITE          0x0001 // Indicate plain write sequence: S+ADDR(W)+DATA0+P.
#define I2C_FLAG_READ           0x0002 // Indicate plain read sequence: S+ADDR(R)+DATA0+P.
#define I2C_FLAG_WRITE_READ     0x0004 // Indicate combined write/read sequence: S+ADDR(W)+DATA0+Sr+ADDR(R)+DATA1+P.
#define I2C_FLAG_WRITE_WRITE    0x0008 // Indicate write sequence using two buffers: S+ADDR(W)+DATA0+DATA1+P.
#define MCU_ID                  0xa0   // image processing mcu address

typedef enum{
    i2cMasterIdle,
    i2cMasterTransmit,
    i2cMasterReceive,
    i2cMasterErr


}I2C_State_t;

typedef enum {

  i2cTransferInProgress = 1,    //Transfer in progress. In progress code (>0)
  i2cTransferDone       = 0,    //Transfer completed successfully. Complete code (=0)

  // Transfer error codes (<0)
  i2cTransferDataNack   = -1,   // NACK received during transfer
  i2cTransferAddrNack   = -2,   // NACK for slave address
  i2cTransferBusErr     = -3,   // Bus error during transfer (misplaced START/STOP)
  i2cTransferArbLost    = -4,   // Arbitration lost during transfer
  i2cTransferUsageFault = -5,   // Usage fault
  i2cTransferSwFault    = -6,   // SW fault
  i2cError              = -7,   // Figure it out
  i2cTimeoutErr         = -8    // Timeout error
} I2C_TransferReturn_t;


typedef struct {

    I2C_State_t state;      // Current state
    I2C_TransferReturn_t result;  // Result return code
    uint8_t addr;    // Address of slave
    uint8_t flag;    // Flags defining sequence type
    uint8_t reg;
    uint8_t index;
    uint8_t *data;
    uint16_t length;

} I2C_Transfer_t;

void Init_I2C();
I2C_TransferReturn_t I2C_Read_Byte(uint8_t addr, uint8_t reg,uint8_t *data);
I2C_TransferReturn_t I2C_Write_Byte(uint8_t addr, uint8_t reg, uint8_t data);
uint32_t Get_I2C_Status(I2C0_Type *i2c);

#endif /* I2C_H_ */
