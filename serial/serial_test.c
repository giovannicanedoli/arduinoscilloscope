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

//Struct used to send data
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

        if (data.channels < 1 || data.channels > 8) {
            printf("Not a valid value for channels. Must be in the range 1-8\n");
            return 1;
        }

        data.mode = atoi(argv[2]);

        if (data.mode < 1 || data.mode > 2) {
            printf("Not a valid value for mode. Must be either 1 or 2\n");
            print_help();
            return 1;
        }

        freq = atoi(argv[3]);

        if (freq <= 0) {
            printf("Not a valid value for frequency. Must be a positive integer.\n");
            print_help();
            return 1;
        }

        data.frequency = freq;

        if(data.frequency > 1000){
            printf("BEWARE: having a value above 1000 could cause trubles\nConfirm? [y/N] ");
            print_help();
            fflush(stdout);

            int c = getchar();    
            if (c == 'y' || c == 'Y') {
                
            } else {
                return 1;
            }
        }

        //if given, specify serial_device
        if (argc == 5) {
            strncpy(serial_device, argv[4], sizeof(serial_device) - 1);
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
            printf("Enter mode, a number between 1 and 2 (1 = continuous, 2 = buffered): ");
            scanf("%hhd", &data.mode);
            if(data.mode < 1 || data.mode > 2) {
                printf("Not a valid answer, number must be between either 1 or 2\n");
            }
        } while(data.mode < 1 || data.mode > 2);

        //frequency
        do {
            printf("Enter frequency, positive integer representing timer interrupt time: ");
            scanf("%d", &freq);
            if (freq <= 0) {
                printf("Not a valid answer, must be a positive integer.\n");
            }

            if(freq > 1000){
                printf("BEWARE: having a value above 1000 could cause trubles\nConfirm? [y/N] ");
                fflush(stdout);
                while (getchar() != '\n'); // discard leftover newline character
                int c = getchar();    
                if (c == 'y' || c == 'Y') {
                    break;
                } else {
                    return 1;
                }
            }

        } while (freq <= 0);

        data.frequency = (uint16_t)freq;

        //device_path
        printf("Enter the path for your serial device (default: /dev/ttyACM0): ");
        scanf("%s", serial_device);
        if (strlen(serial_device) == 0) {
            strncpy(serial_device, "/dev/ttyACM0", sizeof(serial_device) - 1);
        }
    }
  
    //handling SIGINT
    signal(SIGINT, sigintHandler); 

    //setup filenames array
    char filenames[NFILES][20];
    for (int i = 0; i < NFILES; i++){
        sprintf(filenames[i], "datafile%d", i);
    }

    //setup files array
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

    int mode = WRITE;
    uint8_t buf[BUF_SIZE];
    int nchars;
    int file_num;
    int matches = 0, counter = -1;  //counter is -1 since the first string is [ARDUINO] RICEVUTO 
    char adc_value[10];
    float volts;
    char* line;
    printf("%u\n", data.channels);
    printf("Ready!\n");
    while(1) {
    memset(buf, 0, BUF_SIZE);   //clearing buffer used to receive data
        if (mode == READ) {
            
            nchars=read(serial_fd, buf,BUF_SIZE);
            usleep(10000);

            //I start plot.sh only if at least data.channels samples are arrived
            if(counter == data.channels){
                        sprintf(buf, "./plot.sh %d", data.channels);
                        system(buf);    //start plot.sh with data.channels as a parameter
            }

            //Extracting data from buf
            line = strtok(buf, "\n");

            while(line != NULL){
                    printf("%s\n", line);
                    matches = sscanf(line, "%d %f", &file_num, &volts); //extracting data
                    if(matches < 2)break;   //first check
                    volts = ((volts + 1) * 5) / 1024;   //conversion in volts
                    sprintf(adc_value, "%f\n", volts);
                    if(file_num < 0 || file_num >= 8) break; //another check
                    fputs(adc_value, files[file_num]);  //writing on file[index]
                    fflush(files[file_num]);    //ensuring data is written
                    line = strtok(NULL, "\n");
                }

            counter++;
            usleep(1000000); //waiting 1s
        } else {
            usleep(1000000); //waiting 1s
            serial_send(&data, serial_fd);
            mode = READ;
            printf("Data sent...\n");
            printf("I'm listening...\n");
            if(data.mode == 2){
                printf("Remember that arduino is waiting for an interrupt!\n");
            }
            usleep(5000);   //waiting 0.5s
        }
    }
    return 0;
}

/* Sending data over serial */
void serial_send(const Data* data, int serial_fd){
    uint8_t send_buffer[sizeof(Data)];
    //serialize data
    memcpy(send_buffer, data, sizeof(Data));
    //using write syscall to send data with serial_fd
    ssize_t count = write(serial_fd, send_buffer,sizeof(Data));
    return;

}

/* Signal Handler for SIGINT */
void sigintHandler(int sig_num){
    signal(SIGINT, sigintHandler); 
    int ret;
    printf("SIGINT captured.\nClosing communication with arduino... ");
    fflush(stdout); 
    ret = close(serial_fd);
    if(ret == -1){
        perror("ERROR WHILE TRYING TO CLOSE SERIAL FD");
        exit(EXIT_FAILURE);
    }
    printf("DONE\n");
    printf("Closing all the files... ");
    fflush(stdout); 
    for (int i = 0; i < NFILES; i++) {
        if((ret = fclose(files[i])) == EOF){
            perror("ERROR WHILE TRYING TO CLOSE FILES");
            exit(EXIT_FAILURE);
        }
    }
    printf("DONE\n");
    printf("Killing gnuplot processes... ");
    fflush(stdout); 
    system("pkill gnuplot");
    fflush(stdout); 
    printf("DONE\n");
    printf("Exiting, have a nice day :)\n");
    exit(0);
} 

/* --help */ 
void print_help() {
    printf("Usage: program [channels] [mode] [frequency] [device_path]\n");
    printf("    channels: a number between 1 and 8\n");
    printf("    mode: a number between 1 and 2 (1 = continuous, 2 = buffered)\n");
    printf("    frequency: a non negative number representing timer interrupt time\n");
    printf("    device_path: device path (optional, default: /dev/ttyACM0)\n");
    printf("If no arguments are provided, interactive input will be requested.\n");
    printf("Options:\n");
    printf("    --help       Show this help message and exit\n");
}
