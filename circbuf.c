/*
 * circbuf.c
 *
 *  Created on: Jan 15, 2020
 *      Author: samaustin
 */

#include "circbuf.h"
#include <stdlib.h>
#include "msp.h"
#include "core_cm4.h"
uint8_t* initialize_buffer(CircBuf_t * buf, uint32_t length){
    buf->buffer =(uint16_t*)malloc(sizeof(uint16_t)*length);
    if(buf->buffer == 0){
        return 0;
    }
    buf->length = length;
    buf->num_items = 0;
    buf->head = buf->buffer;
    buf->tail = buf->buffer;
    buf->state = standbye;
    return 0;
}

void clear_buffer(CircBuf_t * buf){
    int i;
    for(i=0; i < buf->length; i++){
        buf->buffer[i] = 0;
    }
    buf->num_items = 0;
    buf->head = buf->buffer;
    buf->tail = buf->buffer;
}

void delete_buffer(CircBuf_t * buf){
    free(buf->buffer);
}

int buffer_full(CircBuf_t * buf){
    if(buf->num_items == buf->length){
        return 1;
    }
    return 0;
}

int buffer_empty(CircBuf_t * buf){
    if(buf->num_items == 0){
        return 1;
    }
    return 0;
}

void add_item_to_buffer(CircBuf_t * buf, uint16_t item){

    if(buffer_empty(buf)==1){ //if buffer is empty
        *(buf->tail) = item; //set tail to new item
        *(buf->head)=item;   //set head to new item
        (buf->num_items)++;  //states that there is now one item
    }
    else if(buffer_full(buf) == 0)
    {
        (buf->tail)++;       //move tail up one position
       *(buf->tail) = item; //set new item to the tail
        (buf->num_items)++;  //number of items goes up one
    }
    else if(buf->tail == (buf->buffer+(buf->length-1))){ //if tail at the end of buffer
        buf->tail = buf->buffer; //move tail back to beginning of buffer
        (buf->head)++;           //move the head up to the second oldest value
        *(buf->tail) = item;     //clears oldest value with the new item
    }
    else if(buf->head == (buf->buffer+(buf->length-1))){ //if head is at end of buffer
        buf->head =buf->buffer; //head moves back to beginning of buffer
        (buf->tail)++;          //moves the tail up to the last position in buffer
        *(buf->tail) = item;    //clears oldest value with the new item

    }
    else{ //for all other buffer conditions
        buf->tail++;       //move tail over oldest item in buffer
        buf->head++;       //moves head to second oldest item in buffer
        *(buf->tail)=item; //clears oldest value with the new item
    }
}

uint16_t remove_item_from_buffer(CircBuf_t * buf){
    uint16_t item;
    if(buffer_empty(buf)==1){
        clear_buffer(buf);
        return 0;
    }
    else if(buf->num_items == 1){
        item = *(buf->head);
        clear_buffer(buf);
    }
    else if(buf->head == (buf->buffer+(buf->length-1))){ //if tail at the end of buffer
        item = *(buf->head);     // Get item
        buf->head = buf->buffer; // Move head back to beginning of buffer
        buf->num_items--;        // Lower number of items
    }
    else{ // if head is not at the end of buffer
        item = *(buf->head);     // Get item
        buf->head++; // Move head back to beginning of buffer
        buf->num_items--;        // Lower number of items
    }
    return item;
}





