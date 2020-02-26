/*
 * timer.c
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#include "timer.h"

extern volatile Si114x_State_t pirStatus;

void Init_Timers()
{
    TIMER0->CTL &= ~TIMER_CTL_TAEN;         // Disable timer
    //TIMER0->CFG |= TIMER_CFG_16_BIT;      // Configure for 16-bit timer
    TIMER0->TAMR |= TIMER_TAMR_TAMIE;
    TIMER0->TAMR |= TIMER_TAMR_TAMR_1_SHOT; // One-Shot Timer mode
    TIMER0->TAPR = 0x14;                    // Set prescaler to 20
}

void Init_SysTick()
{
    SysTick->LOAD = 1200000 - 1;            // Interrupt ever 100 ms
    SysTick->CTRL = NVIC_ST_CTRL_INTEN;     // Enable SysTick interrupts
}
void SysTickEnable()
{
    SysTick->CTRL |= NVIC_ST_CTRL_CLK_SRC | NVIC_ST_CTRL_ENABLE;
}
void SysTickDisable()
{
    SysTick->CTRL &= ~(NVIC_ST_CTRL_ENABLE);
}

void SysTick_Handler(void)
{

    GPIOH->DATA ^= BIT0; // Toggle GPIO pin

    if(pirStatus == idle)
    {
        pirStatus = getSample;
    }
}


void delay_1ms()
{
    /* TAILR = (SysClkFreq)*(Desired Delay)/(Prescaler) */
    TIMER0->TAILR = 0x1770;          // TAILR = (120 MHz)*(1ms)/(20) = 0x1770
    TIMER0->CTL |= TIMER_CTL_TAEN;   // GPTM Timer A Enable

    /* Wait Until counter has finished */
    while(!(TIMER0->RIS & TIMER_RIS_TATORIS));

    TIMER0->ICR = TIMER_ICR_TATOCINT; // Clear Timeout Flag
}
void delay_10ms()
{
    /* TAILR = (SysClkFreq)*(Desired Delay)/(Prescaler) */
    TIMER0->TAILR = 0xea60;          // TAILR = (120 MHz)*(10ms)/(20) = 0xea60
    TIMER0->CTL |= TIMER_CTL_TAEN;   // GPTM Timer A Enable

    /* Wait Until counter has finished */
    while(!(TIMER0->RIS & TIMER_RIS_TATORIS));

    TIMER0->ICR = TIMER_ICR_TATOCINT; // Clear Timeout Flag
}

