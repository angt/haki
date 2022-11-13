#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#if defined(__linux__)
#include <sys/random.h>
#endif
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/param.h>
#endif

#include "haki.h"

#if defined(__x86_64__)
#define aesenc(X,Y)   _mm_aesenc_si128((X), (Y))
#define load128(X)    _mm_loadu_si128((const void *)(X))
#define store128(X,Y) _mm_storeu_si128((void *)(X), (Y))
#elif defined(__aarch64__)
#define aesenc(X,Y)   veorq_u8(vaesmcq_u8(vaeseq_u8((X), (h128){})), (Y))
#define load128(X)    vld1q_u8((const void *)(X))
#define store128(X,Y) vst1q_u8((void *)(X), (Y))
#endif

struct haki
haki_init(void)
{
    return (struct haki) {
        .b = {
            load128((char[16]){" This Is What I "}),
            load128((char[16]){"   Put In The   "}),
            load128((char[16]){"  State Before  "}),
            load128((char[16]){"  Hashing Your  "}),
            load128((char[16]){"   Stuff With   "}),
            load128((char[16]){"   H A K I !!!  "}),
        },
    };
}

static inline void
haki_round(struct haki *h)
{
    h128 h0 = aesenc(h->b[5], h->b[0]);
    h->b[5] = aesenc(h->b[4], h->b[5]);
    h->b[4] = aesenc(h->b[3], h->b[4]);
    h->b[3] = aesenc(h->b[2], h->b[3]);
    h->b[2] = aesenc(h->b[1], h->b[2]);
    h->b[1] = aesenc(h->b[0], h->b[1]);
    h->b[0] = aesenc(h0, h->b[2] & h->b[4]);
}

void
haki_absorb(struct haki *h, const void *src, size_t size)
{
    size_t i, r = size & 0xF;

    for (i = 0; i + 16 <= size; i += 16) {
        h->b[0] ^= load128((unsigned char *)src + i);
        haki_round(h);
    }
    if (r) {
        _Alignas(16) unsigned char tmp[16] = {0xA0 | r};
        memcpy(tmp + 1, (unsigned char *)src + i, r);
        h->b[0] ^= load128(tmp);
        haki_round(h);
    }
}

void
haki_flip(struct haki *h)
{
    h->b[0] ^= load128((char[16]){[0] = 0x1F});
    haki_round(h);
}

void
haki_squeeze(struct haki *h, void *dst, size_t size)
{
    size_t i, r = size & 0xF;

    for (i = 0; i + 16 <= size; i += 16) {
        haki_round(h);
        store128((unsigned char *)dst + i, h->b[0]);
    }
    if (r) {
        _Alignas(16) unsigned char tmp[16];
        haki_round(h);
        store128(tmp, h->b[0]);
        memcpy((unsigned char *)dst + i, tmp, r);
    }
}

struct haki
haki_init_random(void)
{
    struct haki h;
#if defined(BSD)
    arc4random_buf(&h, sizeof(h));
    goto ok;
#elif defined(__linux__)
    if (getrandom(&h, sizeof(h), 0) == sizeof(h))
        goto ok;
#endif
    h = haki_init();
    time_t  t = time(NULL); haki_absorb(&h, &t, sizeof(t));
    clock_t c = clock();    haki_absorb(&h, &c, sizeof(c));
    pid_t   p = getpid();   haki_absorb(&h, &p, sizeof(p));
ok:
    for (int i = 0; i < 7; i++)
        haki_round(&h);
    return h;
}

unsigned long long
haki_squeeze_n(struct haki *h, unsigned long long limit)
{
    if (limit < 2ULL)
        return 0;

    unsigned long long ret, min = -limit % limit;

    do haki_squeeze(h, &ret, sizeof(ret));
    while (ret < min);

    return ret % limit;
}
