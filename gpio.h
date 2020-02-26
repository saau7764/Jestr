/*
 * gpio.h
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "msp.h"
#include "core_cm4.h"
#include "si114x.h"
#include "timer.h"

void Init_GPIO();
void SetLed(uint8_t led1, uint8_t led2, uint8_t led3, uint8_t led4);

#endif /* GPIO_H_ */
