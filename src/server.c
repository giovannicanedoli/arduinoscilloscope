#include <avr/io.h>
#include <util/delay.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "../avr_common/uart.h"
#include <stdio.h>

void adc_init(void);
uint16_t adc_read(uint8_t ch);

int main(void) {
    printf_init(); 

    adc_init();

    while (1) {
        uint16_t value = adc_read(0);  // read pin A0

        // Convert the value to a string and send it via serial
        char buffer[10];
        itoa(value, buffer, 10);
        for (int i = 0; buffer[i] != '\0'; i++) {
            while (!(UCSR0A & (1 << UDRE0)));  // Wait for the transmit buffer to be empty
            UDR0 = buffer[i];  // Send the character
        }
          
        _delay_ms(1000);
    }
}

void adc_init(void) {
    ADMUX = (1 << REFS0);  // Use AVCC as the voltage reference
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Enable ADC and set prescaler to 128
}

uint16_t adc_read(uint8_t ch) {
    ch &= 0b00000111;  // Mask to allow only the first 8 channels (0-7)
    ADMUX = (ADMUX & 0xF8) | ch;  // Select the ADC channel
    ADCSRA |= (1 << ADSC);  // Start the conversion

    // Wait for the conversion to complete
    while (ADCSRA & (1 << ADSC));

    return (ADC);  // Return the ADC value (10 bit)
}
