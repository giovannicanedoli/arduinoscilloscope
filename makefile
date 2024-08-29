.phony: clean all

all:
	make -C src
	make -C serial

	
clean:
	make -C src clean
	make -C serial clean

