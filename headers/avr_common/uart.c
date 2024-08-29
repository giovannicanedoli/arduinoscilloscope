// ********************************************************************************
// Includes
// ********************************************************************************
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

// ********************************************************************************
// Macros and Defines
// ********************************************************************************
#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1
// ********************************************************************************
// Function Prototypes
// ********************************************************************************


void UART_init(void);
void UART_putChar(uint8_t c);
uint8_t UART_getChar(void);
uint8_t UART_getString(uint8_t* buf);
void UART_putString(uint8_t* buf);
// void printf_init(void);
// int usart_putchar_printf(char var, FILE *stream);

// static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);



// void printf_init(void){
//   stdout = &mystdout;
  
//   // fire up the usart
//   UART_init ();
// }
 
// this function is called by printf as a stream handler
// int usart_putchar_printf(char var, FILE *stream) {
//     // translate \n to \r for br@y++ terminal
//     if (var == '\n') UART_putChar('\r');
//     UART_putChar(var);
//     return 0;
// }
