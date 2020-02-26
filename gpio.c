/*
 * gpio.c
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#include "gpio.h"

extern volatile Si114x_State_t pirStatus;
extern volatile uint8_t ResumeSampling;
extern volatile uint8_t PauseSampling;

void Init_GPIO()
{
    /* configure LED pins: PN0 = LED, PN1 = LED */
    GPION->DEN  |= BIT0 | BIT1;            // enable LEDs as output
    GPION->DIR  |= BIT0 | BIT1;            // enable digital functions for LEDs
    GPIOF->DEN  |= BIT4 | BIT0;
    GPIOF->DIR  |= BIT4 | BIT0;

    /* UART0 receive and transmit pins */
    GPIOA->PCTL  |= BIT0 | BIT4;           // Setting pin to work with UART peripheral
    GPIOA->AFSEL |= BIT0 | BIT1;           // Selecting UART0 peripheral GPIO pins
    GPIOA->DEN   |= BIT0 | BIT1;           // enable digital functions for UART0 pins

    /* configure I2C0 pins: PB2 = I2C0SCL, PB3 = I2C0SDA, F1 = INT for si114x */
    GPIOB->PCTL |= BIT9 | BIT(13);         // Setting pin to work with I2C0 peripheral (correct)
    GPIOB->AFSEL|= BIT2 | BIT3;            // Selecting I2C0 peripheral gpio pins
    GPIOB->ODR  |= BIT3;                   // I2C0SDA configured as open drain.
    GPIOB->PUR  |= BIT3 | BIT2;            // I2C0 pins weak pullup resistor is enabled
    GPIOB->DR4R |= BIT2 | BIT3;            // set pin to have 4-mA driving current for I2C0'
    GPIOB->DEN  |= BIT2 | BIT3;            // enable digital functions for I2C0

    /* configure INT Pin from si114x at pin G0 */
    //GPIOG->PUR |= BIT0;                     // F1 pins weak pullup resistor is enabled
    GPIOG->ODR |= BIT0;                     // open drain for INT connection
    GPIOG->DEN |= BIT0;                     // enable digital functions for INT

    /* SysTick debug purposes */
    GPIOH->DEN |= BIT0;                     // enable digital functions for H0
    GPIOH->DIR |= BIT0;                     // set to an output pin

    /* configure Buttons */
    GPIOJ->DEN |= BIT0 | BIT1;              // enable digital functions for button
    GPIOJ->PUR |= BIT0 | BIT1;              // enable gpio interrupt for falling edge
    GPIOJ->IM  |= BIT0 | BIT1;
    NVIC_EnableIRQ(GPIOJ_IRQn);

}

void SetLed(uint8_t led1, uint8_t led2, uint8_t led3, uint8_t led4)
{
    if(led1)GPION->DATA |= BIT1;
    else GPION->DATA &= ~BIT1;

    if(led2)GPION->DATA |= BIT0;
    else GPION->DATA &= ~BIT0;

    if(led3)GPIOF->DATA |= BIT4;
    else GPIOF->DATA &= ~BIT4;

    if(led4)GPIOF->DATA |= BIT0;
    else GPIOF->DATA &= ~BIT0;
}


void GPIOJ_IRQHandler()
{
    uint32_t pending;

    pending = GPIOJ->MIS;   // Get pending interrupts
    GPIOJ->ICR = pending;    // Clear interrupts

    if(pending & BIT1)
    {
        ResumeSampling = 1;
        PauseSampling = 0;
    }
    else if(pending & BIT0)
    {
        PauseSampling = 1;
        ResumeSampling = 0;
    }
}
