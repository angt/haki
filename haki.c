#include <string.h>
#include "haki.h"

#if defined(__x86_64__)

#define aesenc(X,Y)   _mm_aesenc_si128((X), (Y))
#define load128(X)    _mm_loadu_si128((const h128 *)(X))
#define store128(X,Y) _mm_storeu_si128((h128 *)(X), (Y))

#elif defined(__aarch64__)

#define aesenc(X,Y)   veorq_u8(vaesmcq_u8(vaeseq_u8((X), (h128){})), (Y))
#define load128(X)    vld1q_u8((const uint8_t *)(X))
#define store128(X,Y) vst1q_u8((uint8_t *)(X), (Y))

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
haki_round(h128 *state)
{
    h128 tmp = aesenc(state[5] , state[0]);
    state[5] = aesenc(state[4] , state[5]);
    state[4] = aesenc(state[3] , state[4]);
    state[3] = aesenc(state[2] , state[3]);
    state[2] = aesenc(state[1] , state[2]);
    state[1] = aesenc(state[0] , state[1]);
    state[0] = tmp ^ (state[1] & state[3]);
}

void
haki_absorb(struct haki *h, const void *src, size_t size)
{
    size_t i, r = size & 0xF;

    for (i = 0; i + 16 <= size; i += 16) {
        h->b[0] ^= load128((unsigned char *)src + i);
        haki_round(h->b);
    }
    if (r) {
        _Alignas(16) unsigned char tmp[16] = {0xA0 | r};
        memcpy(tmp + 1, (unsigned char *)src + i, r);
        h->b[0] ^= load128(tmp);
        haki_round(h->b);
    }
}

void
haki_flip(struct haki *h)
{
    h->b[0] ^= load128((char[16]){[0] = 0x1F});
    for (int i = 0; i < 7; i++)
        haki_round(h->b);
}

void
haki_squeeze(struct haki *h, void *dst, size_t size)
{
    size_t i, r = size & 0xF;

    for (i = 0; i + 16 <= size; i += 16) {
        haki_round(h->b);
        store128((unsigned char *)dst + i, h->b[0]);
    }
    if (r) {
        _Alignas(16) unsigned char tmp[16];
        haki_round(h->b);
        store128(tmp, h->b[0]);
        memcpy((unsigned char *)dst + i, tmp, r);
    }
}
