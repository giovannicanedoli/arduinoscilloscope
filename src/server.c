#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>


#define BAUD 19200
#define MYUBRR (F_CPU/16/BAUD-1)

volatile uint16_t adc_value = 0;  // global variable to memorize adc value
void adc_init(void);


void UART_init(void){
  // Set baud rate
  UBRR0H = (uint8_t)(MYUBRR>>8);
  UBRR0L = (uint8_t)MYUBRR;

  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); /* 8-bit data */ 
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);   /* Enable RX and TX */  

}

void UART_putChar(uint8_t c){
  // wait for transmission completed, looping on status bit
  while ( !(UCSR0A & (1<<UDRE0)) );

  // Start transmission
  UDR0 = c;
}

uint8_t UART_getChar(void){
  // Wait for incoming data, looping on status bit
  while ( !(UCSR0A & (1<<RXC0)) );
  
  // Return the data
  return UDR0;
    
}

// reads a string until the first newline or 0
// returns the size read
uint8_t UART_getString(uint8_t* buf){
  uint8_t* b0=buf; //beginning of buffer
  while(1){
    uint8_t c=UART_getChar();
    *buf=c;
    ++buf;
    // reading a 0 terminates the string
    if (c==0)
      return buf-b0;
    // reading a \n  or a \r return results
    // in forcedly terminating the string
    if(c=='\n'||c=='\r'){
      *buf=0;
      ++buf;
      return buf-b0;
    }
  }
}

void UART_putString(uint8_t* buf){
  while(*buf){
    UART_putChar(*buf);
    ++buf;
  }
}

#define MAX_BUF 256
int main(void){
  UART_init();
    adc_init();
    sei();  // Abilita interrupt globali
    while (1) {
        // // Convert the value to a string and send it via serial
        uint8_t* buffer[10];  // Corretto tipo di buffer come char[]
        
        snprintf((char*)buffer, sizeof(buffer), "%d", adc_value);  // Conversione sicura
        
        UART_putString((uint8_t *)buffer);  // Invia stringa tramite UART
        UART_putString((uint8_t *)"\n");
        memset(buffer, 0, sizeof(buffer));  // Pulisce il buffer
        _delay_ms(1000);
    }
}

void adc_init(void) {
    ADMUX = (1 << REFS0);  // Use AVCC as the voltage reference
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Enable ADC and set prescaler to 128
    ADCSRA |= (1 << ADIE);  // Abilita interrupt ADC
    ADCSRA |= (1 << ADSC);  // Avvia la conversione ADC
}

ISR(ADC_vect) {
    adc_value = ADC;  // Legge valore ADC (10 bit)
    ADCSRA |= (1 << ADSC);  // Avvia una nuova conversione ADC
}
