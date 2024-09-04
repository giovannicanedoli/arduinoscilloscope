#include "serial.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#define READ 1
#define WRITE 0

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
    FILE* file1 = fopen("datafile1", "w");
    FILE* file2 = fopen("datafile2", "w");
    FILE* file3 = fopen("datafile3", "w");
    FILE* file4 = fopen("datafile4", "w");
    FILE* file5 = fopen("datafile5", "w");
    FILE* file6 = fopen("datafile6", "w");
    FILE* file7 = fopen("datafile7", "w");
    FILE* file8 = fopen("datafile8", "w");

    if (file1 == NULL) {
        perror("Error while trying to open first datafile");
        exit(EXIT_FAILURE);
    }
    if (file2 == NULL) {
        perror("Error while trying to open second datafile");
        exit(EXIT_FAILURE);
    }
    if (file3 == NULL) {
        perror("Error while trying to open third datafile");
        exit(EXIT_FAILURE);
    }
    if (file4 == NULL) {
        perror("Error while trying to open fourth datafile");
        exit(EXIT_FAILURE);
    }
    if (file5 == NULL) {
        perror("Error while trying to open fifth datafile");
        exit(EXIT_FAILURE);
    }
    if (file6 == NULL) {
        perror("Error while trying to open sixth datafile");
        exit(EXIT_FAILURE);
    }
    if (file7 == NULL) {
        perror("Error while trying to open seventh datafile");
        exit(EXIT_FAILURE);
    }
    if (file8 == NULL) {
        perror("Error while trying to open eighth datafile");
        exit(EXIT_FAILURE);
    }


    int serial_fd=serial_open(serial_device);

    serial_set_interface_attribs(serial_fd, 19200, 0);
    serial_set_blocking(serial_fd, 1);

    printf("in place\n");
    while(1) {
    memset(buf, 0, BUF_SIZE);
        if (initilized) {
            
            nchars=read(serial_fd, buf,BUF_SIZE);
            printf("%s", buf);
            
            if(data.mode == 1){
                //to avoid writing "RICEVUTO on files"
                if(strncmp(buf, "RICEVUTO!",9)!= 0){
                    fputs(buf, file1);
                    fflush(file1);
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
