CFLAGS ?= -Wall -O2 -maes

test: haki.o

clean:
	rm -f test test.o haki.o

.PHONY: clean
