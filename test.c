#include <stdio.h>
#include <string.h>

#include "haki.h"

int
main(int argc, char **argv)
{
    struct haki h = haki_init();

    for (int i = 1; i < argc; i++)
        haki_absorb(&h, argv[i], strlen(argv[i]));

    haki_flip(&h);

    unsigned char out[16];
    haki_squeeze(&h, out, sizeof(out));

    for (int i = 0; i < 16; i++)
        printf("%02x", out[i]);

    printf("\n");
}
