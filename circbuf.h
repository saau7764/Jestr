/*
 * circbuf.h
 *
 *  Created on: Jan 15, 2020
 *      Author: samaustin
 */

#ifndef CIRCBUF_H_
#define CIRCBUF_H_

#include "circbuf.h"
#include <stdint.h>
#include "msp.h"
#include "defines.h"

typedef enum{
    filling,
    processing,
    threshold,
    standbye
}BufferState_t;

typedef struct{
    uint16_t* buffer; //Pointer to the base of the buffer in heap
    uint16_t* head; //Pointer to first item
    uint16_t* tail; //Pointer to last item
    uint32_t num_items; //Number of items in buffer
    uint32_t length;//Max number of items the buffer can hold
    BufferState_t state; // buffer state
} CircBuf_t;


uint8_t* initialize_buffer(CircBuf_t * buf, uint32_t length);
void clear_buffer(CircBuf_t * buf);
void delete_buffer(CircBuf_t * buf);
int buffer_full(CircBuf_t * buf);
int buffer_empty(CircBuf_t * buf);
void add_item_to_buffer(CircBuf_t * buf, uint16_t item);
uint16_t remove_item_from_buffer(CircBuf_t * buf);

#endif /* CIRCBUF_H_ */
