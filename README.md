# haki
A PoC for a fast but unsafe and hardware-dependent hash function

    $ make
    cc -Wall -O2 -maes   -c -o test.o test.c
    cc -Wall -O2 -maes   -c -o haki.o haki.c
    cc   test.o haki.o   -o test

    $ ./test Hello World
    af12df3e128cf9d9660cabb0e3df8d98
