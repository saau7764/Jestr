/*
 * timer.h
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "msp.h"
#include "si114x.h"

void Init_Timers();
void Init_SysTick();
void SysTickDisable();
void SysTickEnable();
void delay_10ms();
void delay_1ms();

#endif /* TIMER_H_ */
