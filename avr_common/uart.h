#pragma once

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define BAUD 19200
#define MYUBRR (F_CPU/16/BAUD-1)


void UART_init(void);
void UART_putChar(uint8_t c);
void UART_putString(uint8_t* buf);
uint8_t UART_getChar(void);
uint8_t UART_getString(uint8_t* buf);
void UART_getData(uint8_t* buf, uint8_t size);

