# haki
A PoC for a fast but unsafe hash function

    $ make
    cc -Wall -O3 -march=native   -c -o haki.o haki.c
    cc -Wall -O3 -march=native    test.c haki.o   -o test

    $ ./test Hello World
    31ff6a4a58486f6598de1c3d6a1771cb

    $ CC=aarch64-linux-musl-gcc CFLAGS=-march=armv8.1-a+crypto LDFLAGS=-static make
    aarch64-linux-musl-gcc -march=armv8.1-a+crypto   -c -o haki.o haki.c
    aarch64-linux-musl-gcc -march=armv8.1-a+crypto  -static  test.c haki.o   -o test

    $ qemu-aarch64 ./test Hello World
    31ff6a4a58486f6598de1c3d6a1771cb

    $ CC=aarch64_be-linux-musl-gcc CFLAGS=-march=armv8.1-a+crypto LDFLAGS=-static make
    aarch64_be-linux-musl-gcc -march=armv8.1-a+crypto   -c -o haki.o haki.c
    aarch64_be-linux-musl-gcc -march=armv8.1-a+crypto  -static  test.c haki.o   -o test

    $ qemu-aarch64_be ./test Hello World
    31ff6a4a58486f6598de1c3d6a1771cb
