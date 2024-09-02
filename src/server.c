#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../avr_common/uart.h"

typedef struct{
    uint8_t channels;
    uint16_t frequency;
    uint8_t mode;
}__attribute__((packed)) Rcv_Struct;


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
    uint8_t rcv_buffer[sizeof(Rcv_Struct)];
    Rcv_Struct rcv_data = {0};
    int data_rcved = 0;
    while (1) {
        
        if(!data_rcved){

            for(uint8_t i = 0; i < sizeof(Rcv_Struct); i++){
                rcv_buffer[i] = UART_getChar();
            }
            
            memcpy(&rcv_data, rcv_buffer, sizeof(Rcv_Struct));
            data_rcved = 1;
            UART_putString((uint8_t*)"RICEVUTO!\n");


        }else{
            _delay_ms(10000);
            // UART_putString((uint8_t*)rcv_data.frequency);
            // UART_putChar(rcv_data.channels);

            //continuous sampling !CHANNELS MUST BE SET!
            if(rcv_data.mode == 1){

                
                adc_init();
                sei();
                // Convert the value to a string and send it via serial
                uint8_t* data_buffer[10];
                
                snprintf((char*)data_buffer, sizeof(data_buffer), "%d", adc_value);
                
                UART_putString((uint8_t *)data_buffer);  
                UART_putString((uint8_t *)"\n");
                memset(data_buffer, 0, sizeof(data_buffer));


            }else{
                //bo
            }

            
        }

        _delay_ms(500);
    }
}