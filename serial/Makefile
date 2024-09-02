CC=gcc
CXX=g++
CCOPTS=-std=gnu99
BINS=serial

.phony: clean all

all: $(BINS)

serial:   serial.c serial_test.c
		$(CC) -o $@ $^  

clean:
		rm -rf *~ $(BINS) *.pgm
