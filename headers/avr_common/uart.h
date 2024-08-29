#pragma once

#include <stdio.h>


void UART_init(void);
void UART_putChar(uint8_t c);
void UART_putString(uint8_t* buf);
