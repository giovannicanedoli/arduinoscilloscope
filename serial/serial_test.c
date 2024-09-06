#include "serial.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#define READ 1
#define WRITE 0
#define NFILES 8
#define BUF_SIZE 1024

typedef struct Data{
    uint8_t channels;
    uint16_t frequency;
    uint8_t mode;
}__attribute__((packed)) Data;

FILE* files[NFILES];
int serial_fd;


void serial_send(const Data* data, int serial_fd);

void sigintHandler(int sig_num);

int main(int argc, const char** argv) { 
    const char* serial_device="/dev/ttyACM0";
    int initilized = WRITE;

    int nchars;
    uint8_t buf[BUF_SIZE];
    int channels;


    Data data = {0};
    data.channels = 4;
    data.frequency = 12345;   
    data.mode = 1;    
    
    char filenames[NFILES][20];
    for (int i = 0; i < NFILES; i++){
        sprintf(filenames[i], "datafile%d", i);
    }

    files[NFILES];
    for(int i = 0; i < NFILES; i++){
        files[i] = fopen(filenames[i], "w");
        if(files[i] == NULL){
            perror("Error while trying to open datafile");
            exit(EXIT_FAILURE);
        }
    }

    serial_fd=serial_open(serial_device);

    serial_set_interface_attribs(serial_fd, 19200, 0);
    serial_set_blocking(serial_fd, 1);

    //handling SIGINT
    signal(SIGINT, sigintHandler); 

    int file_num;
    int matches = 0, counter = 0;
    char adc_value[10];
    float volts;
    char* line;

    printf("in place\n");
    while(1) {
    memset(buf, 0, BUF_SIZE);
        if (initilized) {
            
            nchars=read(serial_fd, buf,BUF_SIZE);
            // printf("%s\n", buf); //debug
            usleep(10000);
            
            if(data.mode == 1 && strncmp(buf, "RICEVUTO!",9)!= 0){

                line = strtok(buf, "\n");
                //Donno why is sent buffered even though I send one line at a time
                while(line != NULL){
                    printf("%s\n", line);
                    matches = sscanf(line, "%d %f", &file_num, &volts);
                    if(matches < 2)break;
                    volts = ((volts + 1) * 5) / 1024;   //conversion in volts
                    // printf("%d, %f\n", file_num, volts); DEBUG
                    sprintf(adc_value, "%f\n", volts);
                    if(file_num < 0 || file_num >= 8) break; //another check
                    fputs(adc_value, files[file_num]);
                    fflush(files[file_num]);

                    if(counter == data.channels){
                        sprintf(buf, "./plot.sh %d", data.channels);
                        system(buf);
                    }
                    counter++;
                    line = strtok(NULL, "\n");
                }
                
            }else{
                
                line = strtok(buf, "\n");
                while(line != NULL){
                    printf("%s\n", line);
                    matches = sscanf(line, "%d %f", &file_num, &volts);
                    if(matches < 2)break;
                    volts = ((volts + 1) * 5) / 1024;   //conversion in volts
                    // printf("%d, %d\n", file_num, volts); DEBUG
                    sprintf(adc_value, "%f\n", volts);
                    if(file_num < 0 || file_num >= 8) break; //another check
                    fputs(adc_value, files[file_num]);
                    fflush(files[file_num]);
                    if(counter == data.channels){
                        sprintf(buf, "./plot.sh %d", data.channels);
                        system(buf);
                    }
                    counter++;
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
    return 0;
}

void serial_send(const Data* data, int serial_fd){
    uint8_t send_buffer[sizeof(Data)];

    memcpy(send_buffer, data, sizeof(Data));

    ssize_t count = write(serial_fd, send_buffer,sizeof(Data));
    

    return;

}

/* Signal Handler for SIGINT */
void sigintHandler(int sig_num){
    /* Reset handler to catch SIGINT next time. 
    Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler); 
    int ret;
    printf("SIGINT captured.\nClosing communication with arduino... ");
    ret = close(serial_fd);
    if(ret == -1){
        perror("ERROR WHILE TRYING TO CLOSE SERIAL FD");
        exit(EXIT_FAILURE);
    }
    printf("DONE\n");
    printf("Closing all the files... ");
    for (int i = 0; i < NFILES; i++) {
        if(ret = fclose(files[i]) == EOF){
            perror("ERROR WHILE TRYING TO CLOSE FILES");
            exit(EXIT_FAILURE);
        }
    }
    printf("DONE\n");
    printf("Killing gnuplot processes... ");
    system("pkill gnuplot");
    fflush(stdout); 
    printf("DONE\n");
    printf("Exiting, have a nice day :)\n");
    exit(0);
} 
