# haki
A PoC for a fast but unsafe hash function

    $ make
    cc -Wall -O3 -march=native    test.c   -o test

    $ ./test Hello World
    1fd81758adc267e145e3610bae36f9c0

    $ CC=aarch64-linux-musl-gcc CFLAGS=-march=armv8.1-a+crypto LDFLAGS=-static make
    aarch64-linux-musl-gcc -march=armv8.1-a+crypto  -static  test.c   -o test

    $ qemu-aarch64 ./test Hello World
    1fd81758adc267e145e3610bae36f9c0

    $ CC=aarch64_be-linux-musl-gcc CFLAGS=-march=armv8.1-a+crypto LDFLAGS=-static make
    aarch64_be-linux-musl-gcc -march=armv8.1-a+crypto  -static  test.c   -o test

    $ qemu-aarch64_be ./test Hello World
    1fd81758adc267e145e3610bae36f9c0
