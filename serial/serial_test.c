#include "serial.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#define READ 1
#define WRITE 0
#define NFILES 8
#define BUF_SIZE 1024

typedef struct Data{
    uint8_t channels;
    uint16_t frequency;
    uint8_t mode;
}__attribute__((packed)) Data;


void serial_send(const Data* data, int serial_fd);

int main(int argc, const char** argv) { 
    const char* serial_device="/dev/ttyACM0";
    int initilized = WRITE;

    int nchars;
    char buf[BUF_SIZE];
    int channels;


    Data data = {0};
    data.channels = 4;
    data.frequency = 12345;   
    data.mode = 2;    



    // printf("Enter baudrate: \n")
    // int baudrate = fgets(buf, BUF_SIZE, stdin);
    //memset(buf, 0, BUF_SIZE);
    
    
    // printf("Enter number of channels (1-8)");
    // channels = fgetc(stdin); (CONTROLLI SUL NUMERO! se non è valido richiedilo!)

    // printf("Enter the frequency ");
    // 


    //open several datafiles (usare array per risparmiare righe di codice!)

    char filenames[NFILES][20];
    for (int i = 0; i < NFILES; i++){
        sprintf(filenames[i], "datafile%d", i);
    }

    FILE* files[NFILES];
    for(int i = 0; i < NFILES; i++){
        files[i] = fopen(filenames[i], "w");
        if(files[i] == NULL){
            perror("Error while trying to open datafile");
            exit(EXIT_FAILURE);
        }
    }

    // for (int i = 0; i < NFILES; i++) {
        // fclose(files[i]);  somewhere
    // }

    int serial_fd=serial_open(serial_device);

    serial_set_interface_attribs(serial_fd, 19200, 0);
    serial_set_blocking(serial_fd, 1);

    int file_num;
    int matches = 0;

    char adc_value[5];
    int volts;
    char* line;

    printf("in place\n");
    while(1) {
    memset(buf, 0, BUF_SIZE);
        if (initilized) {
            
            nchars=read(serial_fd, buf,BUF_SIZE);
            printf("%s", buf); //debug
            
            
            if(data.mode == 1 && strncmp(buf, "RICEVUTO!",9)!= 0){
                //successivamente voglio scriverli su un file (senza il RICEVUTO)
                matches = sscanf(buf, "%d %d\n", &file_num, &volts);
                if(matches < 2)continue;    //something happened to data
                volts = ((volts + 1) * 5) / 1024;   //conversion in volts
                sprintf(adc_value, "%d\n", volts);
                fputs(adc_value, files[file_num]);
                fflush(files[file_num]);
            }else{
                
                line = strtok(buf, "\n");
                while(line != NULL){
                    matches = sscanf(line, "%d %d", &file_num, &volts);
                    if(matches < 2)break;
                    volts = ((volts + 1) * 5) / 1024;   //conversion in volts
                    // printf("%d, %d\n", file_num, volts);
                    sprintf(adc_value, "%d\n", volts);
                    fputs(adc_value, files[file_num]);
                    fflush(files[file_num]);
                    
                    // Next line
                    line = strtok(NULL, "\n");
                }

            }

            usleep(1000000);
        } else {
            usleep(1000000);
            serial_send(&data, serial_fd);
            initilized = READ;
            printf("Data sent...\n");
            printf("I'm listening...\n");
            usleep(4000);
        }
    }
}

void serial_send(const Data* data, int serial_fd){
    uint8_t send_buffer[sizeof(Data)];

    memcpy(send_buffer, data, sizeof(Data));

    ssize_t count = write(serial_fd, send_buffer,sizeof(Data));
    

    return;

}
