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
void print_help();
void sigintHandler(int sig_num);

int main(int argc, const char** argv) { 
    int channels;
    Data data = {0};
    data.channels;
    data.frequency;   
    data.mode;   
    char serial_device[30]="/dev/ttyACM0";
    int freq;


    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_help();
        return 0;
    }

    if (argc >= 4) {
        data.channels = atoi(argv[1]);
        data.mode = atoi(argv[2]);
        freq = atoi(argv[3]);
        data.frequency = freq;

        if (data.channels < 1 || data.channels > 8) {
            printf("Not a valid value for channels. Must be in the range 1-8\n");
            return 1;
        }
        if (data.mode < 1 || data.mode > 2) {
            printf("Not a valid value for mode. Must be either 1 or 2\n");
            return 1;
        }

        if (data.frequency <= 0) {
            printf("Not a valid value for frequency. Must be a positive integer.\n");
            return 1;
        }

        if(data.frequency > 0 && data.frequency <= 500){
            printf("BEWARE: having a value below 500 ms could cause trubles\nConfirm? [y/N] ");
            int c = getchar();    
            if (c == 'y' || c == 'Y') {
                
            } else {
                return 1;
            }
        }
        

        //if given, specify serial_device
        if (argc == 5) {
            strncpy(serial_device, argv[3], sizeof(serial_device) - 1);
            serial_device[sizeof(serial_device) - 1] = '\0';
        }
    }else if(argc > 1){
        printf("Error: unregular number of parameters\n");
        print_help();
        return 1;
    }else{

        //data.channels
        do {
            printf("Enter number of channels (1-8): ");
            scanf("%hhd", &data.channels);
            if(data.channels < 1 || data.channels > 8) {
                printf("Not a valid answer, number must be between (1-8)\n");
            }
        } while(data.channels < 1 || data.channels > 8);

        //data.mode
        do {
            printf("Enter mode (1-2): ");
            scanf("%hhd", &data.mode);
            if(data.mode < 1 || data.mode > 2) {
                printf("Not a valid answer, number must be between either 1 or 2\n");
            }
        } while(data.mode < 1 || data.mode > 2);

        do {
            printf("Enter frequency (ms) (positive integer): ");
            //change! use freq
            scanf("%hd", &data.frequency);
            if (data.frequency <= 0) {
                printf("Not a valid answer, must be a positive integer.\n");
            }
            if(data.frequency > 0 && data.frequency <= 500){
                printf("BEWARE: having a value below 500 ms could cause trubles\nConfirm? [y/N] ");
                int c = getchar();    
                if (c == 'y' || c == 'Y') {
                    break;
                } else {
                    return 1;
                }
            }
        } while (data.frequency <= 0);


        //device_path
        printf("Enter the path for your serial device (default: /dev/ttyACM0): ");
        scanf("%s", serial_device);
        if (strlen(serial_device) == 0) {
            strncpy(serial_device, "/dev/ttyACM0", sizeof(serial_device) - 1);
        }
    }
  
    //handling SIGINT
    signal(SIGINT, sigintHandler); 

    
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

    int initilized = WRITE;
    uint8_t buf[BUF_SIZE];
    int nchars;
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
void print_help() {
    printf("Usage: program [channels] [mode] [frequency] [device_path]\n");
    printf("    channels: a number between 1 and 8\n");
    printf("    mode: a number between 1 and 2\n");
    printf("    frequency: a non negative number\n");
    printf("    device_path: device path (optional, default: /dev/ttyACM0)\n");
    printf("If no arguments are provided, interactive input will be requested.\n");
    printf("Options:\n");
    printf("    --help       Show this help message and exit\n");
}
