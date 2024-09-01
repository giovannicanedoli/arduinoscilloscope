#include "serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ 1
#define WRITE 0
#define MAX_LIMIT 30

void serial_send_data(int fd, const char* data, const char* baudrate){
  char* result = malloc(MAX_LIMIT*2);
  strcpy(result, data);
  strcat(result, ";");
  strcat(result, baudrate);

  printf("length: %ld\n", strlen(result));

  int ret = write(fd, result, strlen(result));
  if(ret == -1){
    perror("Error in sending data!");
    exit(EXIT_FAILURE);
  }
  return;
}



int main(int argc, const char** argv) { 
  const char* serial_device="/dev/ttyACM0";
  char* baudrate_input;
  int read_or_write=WRITE;

  char data[MAX_LIMIT];

  // printf("Enter the active channels you wanna have divided by a comma (1,3,5)\n");
  //controllare se la dimensione dell'input superi MAX_LIMIT
  // fgets(data, MAX_LIMIT, stdin);
  //settare un default
  //fare in modo che si possa avere la lista delle possibili opzioni disponibili in input e se l'input
  //non è quello nella lista c'è un errore
  // printf("Enter the sampling frequency you would like to have (default: 9600): ");
  // fgets(baudrate_input, MAX_LIMIT, stdin);
  // int baudrate = atoi(baudrate_input);

  FILE* file = fopen("datafile", "w");

  if (file == NULL) {
    perror("Error while trying to open datafile");
    exit(EXIT_FAILURE);
  }


  int fd=serial_open(serial_device);
  serial_set_interface_attribs(fd, 19200, 0);
  serial_set_blocking(fd, 1);

  // serial_send_data(data, baudrate_input);
  
  int nchars;
  char buf[1024];
  
  printf("in place\n");
  while(1) {
    memset(buf, 0, 1024);
    if (read_or_write) {
      printf("other branch\n");
      nchars=read(fd, buf,1024);
      printf("%s", buf);
      fputs(buf, file);
      fflush(file);
      // read_or_write = WRITE;
    } else {
      printf("Enter input... ");
      // fgets(buf, 1024, stdin);
      // write(fd, buf, l);
      int c = getc(stdin);
      write(fd, &c, 4);
      read_or_write=READ;
    }
  }
}
