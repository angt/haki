# haki
A PoC for a fast but unsafe and hardware-dependent hash function

    $ make
    cc -Wall -O2 -maes   -c -o test.o test.c
    cc -Wall -O2 -maes   -c -o haki.o haki.c
    cc   test.o haki.o   -o test

    $ ./test Hello World
    9f158f8793aa2570bf2889d14c9d3d27

    $ CC=aarch64-linux-musl-gcc CFLAGS=-march=armv8.1-a+crypto LDFLAGS=-static make
    aarch64-linux-musl-gcc -march=armv8.1-a+crypto   -c -o haki.o haki.c
    aarch64-linux-musl-gcc -march=armv8.1-a+crypto  -static  test.c haki.o   -o test

    $ ./test Hello World
    9f158f8793aa2570bf2889d14c9d3d27
