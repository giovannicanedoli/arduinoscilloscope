#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../avr_common/uart.h"

volatile uint16_t adc_value = 0;  // global variable to memorize adc value

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

