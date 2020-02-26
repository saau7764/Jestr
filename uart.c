/*
 * uart.c
 *
 *  Created on: Jan 28, 2020
 *      Author: samaustin
 */

#include "uart.h"

void Init_UART()
{
    // disable UART0
    UART0->CTL &= ~(UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE);
    UART0->LCRH &= ~UART_LCRH_FEN;

    // Wait until UART0 not busy
    while(UART0->FR & UART_FR_BUSY);

    // 115200 baud
    UART0->IBRD = 0x41;
    UART0->FBRD = 0x7;
    UART0->LCRH = UART_LCRH_WLEN_M | UART_LCRH_FEN;

    // RX enable and UART0 enable
    UART0->CTL = UART_CTL_TXE | UART_CTL_RXE | UART_CTL_UARTEN;

}

void UART_SendGestureFeatures(Gesture_Features_t *gestureFeatures)
{
    int i;
    int features[12];
    char tx_buffer[100];
    int buffer_index = 0;

    for(i = 0; i<3; i++)
    {
        features[i + 0] = gestureFeatures->peakTime[i];
        features[i + 3] = gestureFeatures->peakEntryTime[i];
        features[i + 6] = gestureFeatures->peakExitTime[i];
        features[i + 9] = gestureFeatures->peakAmplitude[i];
    }

    for(i = 0; i<12; i++)
    {
        itoa(features[i], tx_buffer + buffer_index, 10);
        buffer_index = strlen(tx_buffer);
        if(i != 11) tx_buffer[buffer_index++] = ',';
        else tx_buffer[buffer_index++] = '\n';
    }

    for(i = 0; i<buffer_index; i++)
    {
        UART_SendByte(tx_buffer[i]);
    }



}

void itoa(long unsigned int value, char* result, int base)
    {
      // check that the base if valid
      if (base < 2 || base > 36) { *result = '\0';}

      char* ptr = result, *ptr1 = result, tmp_char;
      int tmp_value;

      do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
      } while ( value );

      // Apply negative sign
      if (tmp_value < 0) *ptr++ = '-';
      *ptr-- = '\0';
      while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
      }

    }
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}


void UART_SendByte(uint8_t data)
{
    // Wait until space is available.
    while(UART0->FR & UART_FR_TXFF);
    UART0->DR = data;

}
