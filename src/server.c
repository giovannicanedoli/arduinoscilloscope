#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../avr_common/uart.h"

#define MAX_CHANNELS 8
#define ITERATIONS 10

typedef struct{
    uint8_t channels;
    uint16_t frequency;
    uint8_t mode;
}__attribute__((packed)) Rcv_Struct;

volatile uint16_t adc_value[MAX_CHANNELS];  // Here I have all the converted values
volatile Rcv_Struct rcv_data = {0};
volatile uint8_t trigger=0;
volatile uint8_t sent = 0;
volatile uint8_t current_channel = 0;



void select_adc_channel(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  // Set the channel
}

void volatile_memcpy(volatile void *dest, const void *src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((volatile char*)dest)[i] = ((const char*)src)[i];
    }
}

void adc_init(void) {
    ADMUX = (1 << REFS0);  // Use AVCC as the voltage reference
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Enable ADC and set prescaler to 128
    ADCSRA |= (1 << ADIE);  // enables ADC interrupt
    select_adc_channel(current_channel);  
}

void setup_timer1() {
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = (uint16_t)(15.62 * rcv_data.frequency);  // (16MHz / 1024) * 1s - 1 = 15624 for 1 second => ~15,62 * 10000 = 1 sampling per second
    TIMSK1 = (1 << OCIE1A);  // Enable Timer1 compare match interrupt
}

void setup_interrupt21(){
    DDRD = 0x0;
    PORTD = 0x1;
    // enable interrupt 0
    EIMSK |=1<<INT0;
}

ISR(TIMER1_COMPA_vect) {
    select_adc_channel(current_channel);  // Select the current channel
    ADCSRA |= (1 << ADSC);  // Start a new conversion
}

ISR(ADC_vect) {
    adc_value[current_channel] = ADC;
    if(current_channel >= rcv_data.channels){
        current_channel = 0;
    }else{
        current_channel++;
    }
}

ISR(INT0_vect) {
  trigger=1;
}



int main(void){
    UART_init();
    cli();
    uint8_t rcv_buffer[sizeof(Rcv_Struct)];
    int data_rcved = 0;
    while (1) {
        
        if(!data_rcved){

            for(uint8_t i = 0; i < sizeof(Rcv_Struct); i++){
                rcv_buffer[i] = UART_getChar();
            }
            
            volatile_memcpy(&rcv_data, rcv_buffer, sizeof(Rcv_Struct));
            data_rcved = 1;
            UART_putString((uint8_t*)"[ARDUINO] RICEVUTO!\n");
            _delay_ms(1000);

        }else{
            adc_init();
            setup_timer1();
            setup_interrupt21();
            sei();

            //continuous sampling
            if(rcv_data.mode == 1){

                // Convert the value to a string and send it via serial
                uint8_t data_buffer[50] = {0};

                for(uint8_t i = 0; i < rcv_data.channels; i++){
                    snprintf((char*)data_buffer, sizeof(data_buffer), "%u %u\n", i, adc_value[i]);
                    UART_putString(data_buffer); 
                    memset(data_buffer, 0, sizeof(data_buffer));
                    _delay_ms(500);
                    
                }
                


            }else{
                
                //buffered mode, I make 10 reads from all the channels
                if(!sent){    
                    while (!trigger);   //busy waiting 
                    UART_putString((uint8_t*)"Started sampling in buffered mode!\n");
                    _delay_ms(500);
                    sent = 1;
                }
                
                uint8_t* data_buffer[1000];
                int buff_size = sizeof(data_buffer);
                int written = 0;
                int remaining = buff_size;
                int current_data_written;
                for(uint8_t i = 0; i < ITERATIONS ; i++){
                    for(uint8_t j = 0; j < rcv_data.channels; j++){
                        current_data_written = snprintf((char*)data_buffer + written, remaining, "%u %u\n", j, adc_value[j]);
                        written += current_data_written;
                        remaining -= current_data_written;
                    }
                }
                UART_putString((uint8_t *)data_buffer);
                memset(data_buffer, 0, sizeof(data_buffer));
            }

            
        }
        _delay_ms(500);

    }
}