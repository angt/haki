#include <string.h>
#include "haki.h"

#if defined(__x86_64__)

typedef __m128i       h128;
#define aesenc(X,Y)   _mm_aesenc_si128((X), (Y))
#define xorand(X,Y,Z) _mm_xor_si128((Z), _mm_and_si128((X), (Y)))
#define load128(X)    _mm_loadu_si128((const h128 *)(X))
#define store128(X,Y) _mm_storeu_si128((h128 *)(X), (Y))

#elif defined(__aarch64__)

typedef uint8x16_t    h128;
#define aesenc(X,Y)   veorq_u8(vaesmcq_u8(vaeseq_u8((X), (h128){})), (Y))
#define xorand(X,Y,Z) veorq_u8((Z), vandq_u8((X), (Y)))
#define load128(X)    vld1q_u8((const uint8_t *)(X))
#define store128(X,Y) vst1q_u8((uint8_t *)(X), (Y))

#endif

struct haki
haki_init(void)
{
    static const union {
        struct haki h;
        unsigned char b[sizeof(struct haki)];
    } tmp = {
        .b = {
            0x34, 0x45, 0xd9, 0x3a, 0xfe, 0x9c, 0xc6, 0xcc, 0x04, 0x7f, 0x47, 0x0e,
            0xe4, 0x51, 0xeb, 0xbb, 0x93, 0x06, 0x39, 0x3c, 0x25, 0xa3, 0xeb, 0xb0,
            0x51, 0x31, 0xa6, 0xb2, 0x09, 0x04, 0xef, 0x50, 0xac, 0xdd, 0xa5, 0x74,
            0xc2, 0x32, 0xc1, 0x36, 0x0f, 0xa5, 0x08, 0x28, 0x01, 0xc3, 0x88, 0x53,
            0x8a, 0x06, 0xc0, 0xd1, 0xd2, 0x23, 0x1a, 0x52, 0x57, 0x82, 0x81, 0x95,
            0x09, 0x4e, 0xbf, 0xe6, 0xc0, 0x43, 0x0e, 0xb1, 0x04, 0xa5, 0x01, 0x0e,
            0x0f, 0xd0, 0xb4, 0x69, 0xa5, 0x2f, 0x47, 0x39, 0x99, 0xe5, 0x96, 0x77,
            0xbe, 0xae, 0x96, 0x7e, 0x3b, 0x31, 0x73, 0x19, 0x56, 0x29, 0xeb, 0xf0,
        },
    };
    return tmp.h;
}

static inline void
haki_round(h128 *const state)
{
    for (int i = 0; i < 3; i++) {
        h128 tmp = aesenc(state[5], state[0]);
        state[5] = aesenc(state[4], state[5]);
        state[4] = aesenc(state[3], state[4]);
        state[3] = aesenc(state[2], state[3]);
        state[2] = aesenc(state[1], state[2]);
        state[1] = aesenc(state[0], state[1]);
        state[0] = xorand(state[2], state[3], tmp);
    }
}

void
haki_absorb(struct haki *h, const void *src, size_t size)
{
    size_t i, r = size & 0xF;

    for (i = 0; i + 16 <= size; i += 16) {
        h->b[0] = load128((unsigned char *)src + i);
        haki_round(h->b);
    }
    if (r) {
        _Alignas(16) unsigned char tmp[16] = {0xF0 | r};
        memcpy(tmp + 1, (unsigned char *)src + i, r);
        h->b[0] = load128(tmp);
        haki_round(h->b);
    }
}

void
haki_flip(struct haki *h)
{
    const unsigned char usep = 0x1F;
    memcpy(&h->b[0], &usep, 1);
}

void
haki_squeeze(struct haki *h, void *dst, size_t size)
{
    size_t i, r = size & 0xF;

    for (i = 0; i + 16 <= size; i += 16) {
        haki_round(h->b);
        store128(dst + i, h->b[0]);
    }
    if (r) {
        _Alignas(16) unsigned char tmp[16];
        haki_round(h->b);
        store128(tmp, h->b[0]);
        memcpy((unsigned char *)dst + i, tmp, r);
    }
}
