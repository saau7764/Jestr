/*
 * sysinit.c
 *
 *  Created on: Dec 7, 2019
 *      Author: samaustin
 */

#include "sysinit.h"

void Init_System()
{
    //disable watch dog timers
    SYSCTL->RCGCWD &= ~SYSCTL_RCGCWD_R0;
    SYSCTL->RCGCWD &= ~SYSCTL_RCGCWD_R1;

    /* Set up system clock at 120 MHz */
    SYSCTL->RSCLKCFG |= SYSCTL_RSCLKCFG_USEPLL;      // Clock source is specified by the PLL
    SYSCTL->RSCLKCFG |= SYSCTL_RSCLKCFG_PLLSRC_MOSC; // MOSC is the PLL input clock source
    SYSCTL->RSCLKCFG |= 0x1;                         // Setting PSYSDIV = 1

    SYSCTL->MOSCCTL |= SYSCTL_MOSCCTL_OSCRNG;       // Set MOSC to High-frequency range
    SYSCTL->MOSCCTL &= ~SYSCTL_MOSCCTL_PWRDN;       // Power to MOSC circuit is enabled.
    SYSCTL->MOSCCTL &= ~SYSCTL_MOSCCTL_NOXTAL;      // No MOSC or Crystal Connected

    while (!(SYSCTL_RIS_MOSCPUPRIS & SYSCTL->RIS)); // Wait for MOSC to reach the expected frequency

    // Setting up memory timing parameters for 120 Mhz clock
    SYSCTL->MEMTIM0 |= SYSCTL_MEMTIM0_FBCHT_3_5
                    |  (5 << SYSCTL_MEMTIM0_FWS_S)
                    |  (5 << SYSCTL_MEMTIM0_EWS_S)
                    |  SYSCTL_MEMTIM0_EBCHT_3_5
                    |  SYSCTL_MEMTIM0_MB1;
    SYSCTL->MEMTIM0 &= ~(SYSCTL_MEMTIM0_EBCE | SYSCTL_MEMTIM0_FBCE);

    SYSCTL->PLLFREQ0 = 0x800060; // Supply Power to PLL and Set MINT to 0x60
    SYSCTL->PLLFREQ1 = 0x104;    // Set Q = 1 and N = 4

    SYSCTL->RSCLKCFG |= SYSCTL_RSCLKCFG_NEWFREQ; // Memory Timing Register Update
    SYSCTL->RSCLKCFG |= SYSCTL_RSCLKCFG_MEMTIMU; // New PLLFREQ Accept



    /* Specify which peripherals to clock */

    SYSCTL->RCGCUART |= SYSCTL_RCGCUART_R0;  // Enable and provide a clock to UART0 in run mode.
    while (!(SYSCTL_PPUART_P0 & SYSCTL->PPUART)); // Wait for UART0 to be ready

    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R0;  // Enable and provide a clock to GPIO port A in run mode.
    while (!(SYSCTL_PPGPIO_P0 & SYSCTL->PPGPIO)); // Wait for port A to be ready

    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R1;  // Enable and provide a clock to GPIO port B in run mode.
    while (!(SYSCTL_PPGPIO_P1 & SYSCTL->PPGPIO)); // Wait for port B to be ready

    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R6;  // Enable and provide a clock to GPIO port G in run mode.
    while (!(SYSCTL_RCGCGPIO_R6 & SYSCTL->PPGPIO)); // Wait for port b to be ready

    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R7;  // Enable and provide a clock to GPIO port G in run mode.
    while (!(SYSCTL_RCGCGPIO_R7 & SYSCTL->PPGPIO)); // Wait for port b to be ready

    /* Specify which peripherals to clock */
    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R8;  // Enable and provide a clock to GPIO port J in run mode.
    while (!(SYSCTL_PPGPIO_P8 & SYSCTL->PPGPIO)); // Wait for port J to be ready

    SYSCTL->RCGCGPIO |= SYSCTL_RCGCGPIO_R5;  // Enable and provide a clock to GPIO port F in run mode.
    while (!(SYSCTL_PPGPIO_P5 & SYSCTL->PPGPIO)); // Wait for port F to be ready

    SYSCTL->RCGCGPIO  |= SYSCTL_RCGCGPIO_R12; // Enable and provide a clock to GPIO port N in run mode.
    while (!(SYSCTL_PPGPIO_P12 & SYSCTL->PPGPIO)); // Wait for port N to be ready

    SYSCTL->RCGCI2C  |= SYSCTL_RCGCI2C_R0;   // I2C Module 0 Run Mode Clock enable
    while (!(SYSCTL_PPI2C_P0 & SYSCTL->PPI2C)); // Wait for I2C0 to be ready

    SYSCTL->RCGCTIMER |= SYSCTL_RCGCTIMER_R0;  // Timer Module 0 Run Mode Clock enable
    while (!(SYSCTL_PPTIMER_P0 & SYSCTL->PPTIMER)); // Wait for Timer0 to be ready
}

// wait until we need to use it
//SYSCTL->RCGCGPIO  |= SYSCTL_RCGCGPIO_R10; // Enable and provide a clock to GPIO port L in run mode.
//while (!(SYSCTL_PPGPIO_P10 & SYSCTL->PPI2C)); // Wait for port L to be ready

//SYSCTL->RCGCI2C  |= SYSCTL_RCGCI2C_R0;   // I2C Module 0 Run Mode Clock enable
//while (!(SYSCTL_PPI2C_P0 & SYSCTL->PPI2C)); // Wait for I2C0 to be ready
