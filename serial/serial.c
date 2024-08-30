#include "serial.h"
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 

#define READ 1
#define MAX_LIMIT 30


int serial_set_interface_attribs(int fd, int speed, int parity) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) {
    printf ("error %d from tcgetattr", errno);
    return -1;
  }
  switch (speed){
  case 19200:
    speed=B19200;
    break;
  case 57600:
    speed=B57600;
    break;
  case 115200:
    speed=B115200;
    break;
  case 230400:
    speed=B230400;
    break;
  case 576000:
    speed=B576000;
    break;
  case 921600:
    speed=B921600;
    break;
  default:
    printf("cannot sed baudrate %d\n", speed);
    return -1;
  }
  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);
  cfmakeraw(&tty);
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);               // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;      // 8-bit chars

  if (tcsetattr (fd, TCSANOW, &tty) != 0) {
    printf ("error %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}

void serial_set_blocking(int fd, int should_block) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) {
      printf ("error %d from tggetattr", errno);
      return;
  }

  tty.c_cc[VMIN]  = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
    printf ("error %d setting term attributes", errno);
}

int serial_open(const char* name) {
  int fd = open (name, O_RDWR | O_NOCTTY | O_SYNC );
  if (fd < 0) {
    printf ("error %d opening serial, fd %d\n", errno, fd);
  }
  return fd;
}

void serial_send(const char* data, const char* baudrate){
  return;
}

int main(int argc, const char** argv) { 
  const char* serial_device="/dev/ttyACM0";
  char* baudrate_input;
  int read_or_write=READ;

  char data[MAX_LIMIT];
  printf("Enter the active channels you wanna have divided by a comma (1,3,5)\n");
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
      nchars=read(fd, buf,1024);
      printf("%s", buf);
      fputs(buf, file);
      fflush(file);
    } else {
      fgets(buf, 1024, stdin);
      int l=strlen(buf);
      buf[l]='\n';
      ++l;
      write(fd, buf, l);
    }
  }
}
