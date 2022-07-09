CFLAGS ?= -Wall -O3 -march=native

test: haki.o

clean:
	rm -f test test.o haki.o

.PHONY: clean
