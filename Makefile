CFLAGS=-Wall -O2 -maes

test: test.o haki.o

clean:
	rm -f test test.o haki.o

.PHONY: clean
