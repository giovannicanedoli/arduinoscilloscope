CC=gcc
CXX=g++
CCOPTS=-std=gnu99
BINS=serial

.phony: clean all

all: $(BINS)

serial:   serial.cpp
		$(CXX) -$(CCOPTS) -o $@ $^  

clean:
		rm -rf *~ $(BINS) *.pgm
