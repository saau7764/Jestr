/*
 * uart.h
 *
 *  Created on: Jan 28, 2020
 *      Author: samaustin
 */

#ifndef UART_H_
#define UART_H_

#include "msp.h"
#include "si114x_algorithm.h"
#include "si114x.h"
#include <string.h>
#include "classifier.h"

void Init_UART();
void UART_SendGestureFeatures(Gesture_Features_t *gestureFeatures);
void itoa(long unsigned int value, char* result, int base);
void reverse(char *str, int len);
void UART_SendByte(uint8_t data);

#endif /* UART_H_ */
